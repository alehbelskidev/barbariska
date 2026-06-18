#pragma once

#include <sys/un.h>

#include <functional>
#include <string>

#include "state.hpp"

namespace core {

using UpdateFn = std::function<void(State &)>;

/// This class should use as base for all sub apps clients
class Client {
private:
    int readfd;
    sockaddr_un read_addr;
    std::string read_path;

    /// Callback to update render state after detecting shm_state.version change
    UpdateFn on_state_update;

public:
    /// send cmd to backend, e.g. switch hypr workspace
    void notify(Command cmd);
    void poll_state();
    Client(UpdateFn state_update_cb);
    ~Client();

    int get_readfd() const
    {
        return readfd;
    }
};

}  // namespace core
