#pragma once

#include "state.hpp"

/// Shared memory for application state
class Shm {
private:
    int fd;
    Core::State *shm_ptr;

public:
    Shm();
    ~Shm();
    /// write to shared state
    void write(const Core::State &state);
};
