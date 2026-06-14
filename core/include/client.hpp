#pragma once

#include <functional>

#include "state.hpp"

namespace Core {

using UpdateFn = std::function<void(State &)>;

/// This class should use as base for all sub apps clients
class Client {
private:
    /// Callback to update render state after detecting shm_state.version change
    UpdateFn on_update;

public:
    /// send cmd to backend, e.g. switch hypr workspace
    void notify(Command cmd);
    Client(UpdateFn update_cb);
    ~Client();
};

}  // namespace Core
