#include "client.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <iostream>

Core::Client::Client(UpdateFn update_cb)
    : on_update(update_cb), shm_state(nullptr)
{
    int fd = open(BARBARISKA_SHM_PATH, O_RDONLY, 0600);
    if (fd == -1) {
        std::cerr << "ERROR: Failed to open barbariska shared mem region\n";
        return;
    }

    void *res = mmap(nullptr, sizeof(State), PROT_READ, MAP_SHARED, fd, 0);
    if (res == MAP_FAILED) {
        std::cerr << "ERROR: Failed to map barbariska shared mem region\n";
        shm_state = nullptr;
    } else {
        shm_state = static_cast<State *>(res);
    }

    close(fd);
}

Core::Client::~Client()
{
    munmap(shm_state, sizeof(State));
}

void Core::Client::listen()
{
    if (!shm_state) return;
    if (shm_state->version > version) {
        on_update(*shm_state);
    }
}

void Core::Client::notify(Command cmd)
{
    char path[108];
    snprintf(path, sizeof(path), BARBARISKA_SOCKET_PATH, getuid());

    int fd = socket(AF_LOCAL, SOCK_STREAM | SOCK_CLOEXEC, 0);

    if (fd == -1) {
        std::cerr << "ERROR: Failed to connect to barbariska.sock\n";
        return;
    }

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(fd);
        return;
    }

    write(fd, &cmd, sizeof(cmd));

    close(fd);
}
