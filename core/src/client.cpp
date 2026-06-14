#include "client.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <iostream>

Core::Client::Client(UpdateFn state_update_cb)
    : on_state_update(state_update_cb)
{
    char pathbuf[108];
    snprintf(pathbuf, sizeof(pathbuf), BARBARISKA_SOCKET_READ, getuid());
    read_path = pathbuf;
    readfd = socket(AF_LOCAL, SOCK_STREAM | SOCK_CLOEXEC, 0);

    if (readfd == -1) {
        std::cerr << "ERROR: Failed to connect to barbariska.sock\n";
        return;
    }

    read_addr = {};
    read_addr.sun_family = AF_LOCAL;
    strncpy(read_addr.sun_path, read_path.c_str(),
            sizeof(read_addr.sun_path) - 1);
    read_addr.sun_path[sizeof(read_addr.sun_path) - 1] = '\0';

    if (connect(readfd, (struct sockaddr *)&read_addr, sizeof(read_addr)) == -1)
    {
        close(readfd);
        return;
    }
    fcntl(readfd, F_SETFL, O_NONBLOCK);
    // poll_state();
}

Core::Client::~Client()
{
    if (close(readfd) == -1)
        std::cerr << "ERROR: Failed to close barbariska read .sock\n";
}

void Core::Client::notify(Command cmd)
{
    char path[108];
    snprintf(path, sizeof(path), BARBARISKA_SOCKET_NOTIF, getuid());

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

void Core::Client::poll_state()
{
    char buf[sizeof(Core::State)];
    int n = read(readfd, buf, sizeof(Core::State));
    if (n == -1) {
        std::cerr << "ERROR: Failed to read from barbariska.sock; " << errno
                  << "\n";
        return;
    }
    if (n > 0) {
        auto *state = reinterpret_cast<Core::State *>(buf);
        on_state_update(*state);
    }
}
