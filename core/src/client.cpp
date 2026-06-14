#include "client.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <iostream>

Core::Client::Client(UpdateFn update_cb) : on_update(update_cb)
{
}

Core::Client::~Client()
{
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
