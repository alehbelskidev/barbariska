#include "socket.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>

#define LISTEN_BACKLOG 50

Socket::Socket(UpdateFn update_cb) : on_update(update_cb)
{
    snprintf(sockpath, sizeof(sockpath), BARBARISKA_SOCKET_PATH, getuid());

    fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd == -1) {
        std::cerr << "D_ERROR: Failed to initiate to barbariska.sock\n";
        return;
    }

    addr = {};
    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, sockpath, sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(fd);
        std::cerr << "D_ERROR: Failed to bind to barbariska.sock\n";
        return;
    }

    if (listen(fd, LISTEN_BACKLOG) == -1) {
        close(fd);
        std::cerr << "D_ERROR: Failed to bind to barbariska.sock\n";
        return;
    }

    fcntl(fd, F_SETFL);
}

Socket::~Socket()
{
    if (close(fd) == -1)
        std::cerr << "D_ERROR: Failed to close barbariska.sock\n";
    if (unlink(sockpath) == -1)
        std::cerr << "D_ERROR: Failed to unlink barbariska.sock\n";
}

void Socket::poll_events()
{
    struct sockaddr_un peer_addr;
    socklen_t peer_addr_size = sizeof(peer_addr);
    int cfd = accept(fd, (struct sockaddr *)&peer_addr, &peer_addr_size);

    if (cfd == -1) {
        if (errno != EAGAIN)
            std::cerr << "D_ERROR: Failed to accept from barbariska.sock; "
                      << errno << "\n";
        return;
    }

    size_t bufsize = sizeof(Core::Command);
    char buf[bufsize];

    int n = read(cfd, buf, bufsize - 1);
    if (n == -1) {
        std::cerr << "D_ERROR: Failed to read from barbariska.sock; " << errno
                  << "\n";
        return;
    }
    if (n > 0) {
        const Core::Command *cmd = reinterpret_cast<Core::Command *>(buf);
        on_update(cmd);
    }

    close(cfd);
}
