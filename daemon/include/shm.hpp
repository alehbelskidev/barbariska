#pragma once

#include "state.hpp"

class Shm {
private:
    int fd;
    Core::State *shm_ptr;

public:
    Shm();
    ~Shm();
    void write(const Core::State &state);
};
