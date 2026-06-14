#pragma once

#include <sys/socket.h>
#include <sys/un.h>

#include <functional>

#include "state.hpp"

using UpdateFn = std::function<void(const Core::Command *)>;

class Socket {
private:
    int fd;
    struct sockaddr_un addr;
    UpdateFn on_update;
    char sockpath[108];

public:
    Socket(UpdateFn update_cb);
    ~Socket();

    void poll_events();
    int get_fd() const
    {
        return fd;
    }
};
