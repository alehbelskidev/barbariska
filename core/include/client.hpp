#pragma once

#include <functional>

#include "state.hpp"

namespace Core {

using UpdateFn = std::function<void(State *)>;

/// This class should use as base for all sub apps clients
class Client {
private:
    /// Keeping local version to check against one in shm_state;
    uint64_t version;
    /// Pointer to shared memory
    State *shm_state;
    /// Callback to update render state after detecting shm_state.version change
    ///
    UpdateFn on_update;

public:
    /*
     * Continiously checking inside loop with
     * `Core::WaylandContext::should_dispatch` method just checks for version
     * comparing `shm_state` vs `version`
     */
    void listen();
    /// send cmd to backend, e.g. switch hypr workspace
    void notify(Command cmd);
    Client(UpdateFn update_cb);
    ~Client();

    State *get_state() const
    {
        return shm_state;
    }
};

}  // namespace Core
