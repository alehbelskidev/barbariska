#pragma once

#include <sys/un.h>

#include <functional>
#include <string>
#include <vector>

#include "state.hpp"

using UpdateFn = std::function<void(const Core::Command &)>;

/// Daemon socket handling read/notif
/// ONLY between `barbariska` apps
class BSocket {
private:
    int readfd;
    sockaddr_un read_addr;
    std::string read_path;

    int notiffd;
    sockaddr_un notif_addr;
    std::string notif_path;
    UpdateFn on_notif_update;

    std::vector<int> clientfds;

public:
    BSocket(UpdateFn notif_update_cb);
    ~BSocket();

    void notif_poll_events();
    void accept_client(Core::State &state);
    void broadcast(Core::State &state);

    int get_readfd() const
    {
        return readfd;
    }
    int get_notiffd() const
    {
        return notiffd;
    }
};
