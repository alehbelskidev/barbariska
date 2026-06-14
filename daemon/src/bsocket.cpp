#include "bsocket.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>

#define LISTEN_BACKLOG 50

void init_socket(int &fd, sockaddr_un &addr, std::string &path,
                 const char *cpath)
{
    char buf[108];
    snprintf(buf, sizeof(buf), cpath, getuid());
    path = buf;
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        std::cerr << "D_ERROR: Failed to initiate to barbariska.sock\n";
        return;
    }

    addr = {};
    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    unlink(path.c_str());
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(fd);
        std::cerr << "D_ERROR: Failed to bind: " << errno << " path: " << path
                  << "\n";
        return;
    }

    if (listen(fd, LISTEN_BACKLOG) == -1) {
        close(fd);
        std::cerr << "D_ERROR: Failed to bind to barbariska.sock\n";
        return;
    }
}

BSocket::BSocket(UpdateFn notif_update_cb) : on_notif_update(notif_update_cb)
{
    init_socket(readfd, read_addr, read_path, BARBARISKA_SOCKET_READ);
    init_socket(notiffd, notif_addr, notif_path, BARBARISKA_SOCKET_NOTIF);
}

BSocket::~BSocket()
{
    if (close(readfd) == -1)
        std::cerr << "D_ERROR: Failed to close barbariska read .sock\n";
    if (unlink(read_path.c_str()) == -1)
        std::cerr << "D_ERROR: Failed to unlink barbariska read .sock\n";

    if (close(notiffd) == -1)
        std::cerr << "D_ERROR: Failed to close barbariska notif .sock\n";
    if (unlink(notif_path.c_str()) == -1)
        std::cerr << "D_ERROR: Failed to unlink barbariska notif .sock\n";
}

void BSocket::notif_poll_events()
{
    int clientfd = accept(notiffd, nullptr, nullptr);

    if (clientfd == -1) {
        if (errno != EAGAIN)
            std::cerr << "D_ERROR: Failed to accept from barbariska.sock; "
                      << errno << "\n";
        return;
    }

    char buf[sizeof(Core::Command)];

    int n = read(clientfd, buf, sizeof(Core::Command));
    if (n == -1) {
        std::cerr << "D_ERROR: Failed to read from barbariska.sock; " << errno
                  << "\n";
        return;
    }
    if (n > 0) {
        const Core::Command *cmd = reinterpret_cast<Core::Command *>(buf);
        on_notif_update(*cmd);
    }

    close(clientfd);
}

void BSocket::accept_client(Core::State &state)
{
    int clientfd = accept(readfd, nullptr, nullptr);
    if (clientfd == -1) return;
    write(clientfd, &state, sizeof(state));
    clientfds.push_back(clientfd);
}

void BSocket::broadcast(Core::State &state)
{
    std::erase_if(clientfds, [&](int cfd) {
        auto ret = write(cfd, &state, sizeof(state));
        return ret != sizeof(Core::State);
    });
}
