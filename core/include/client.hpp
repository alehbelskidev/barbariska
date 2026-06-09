#pragma once

#include <functional>

#include "state.hpp"

namespace Core {

using UpdateFn = std::function<void(const State &)>;

class Client {
protected:
    virtual void listen();
    virtual void notify(Command cmd);

private:
    uint64_t version;
    State *shm_state;
    UpdateFn on_update;

public:
    Client(UpdateFn update_cb);
    virtual ~Client();
};

}  // namespace Core
