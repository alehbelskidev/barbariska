#pragma once

#include "surface.hpp"
#include "wlr-layer-shell-unstable-v1-client-protocol.h"

/// SHM buffer pipeline:
/// shm_open -> ftruncate -> mmap -> wl_shm_pool -> wl_buffer
/// COMPOSITOR reads pixels directly from that memory region

class ShmBuffer {
private:
    /// mmap maps that file inot our address space
    /// we write pixels here - COMPOSITOR reads from same physical memory
    void *shm_data;
    /// wl_buffer is a view into the pool
    /// ARGB8888 - 4 bytes per pixel, blue 1st in mem
    wl_buffer *buffer;
    int stride;
    int shm_size;

public:
    ShmBuffer(Surface::Dimensions surface_dimensions, wl_shm *shm);
    ~ShmBuffer();

    void *get_shm_data() const
    {
        return shm_data;
    }

    wl_buffer *get_buffer() const
    {
        return buffer;
    }

    int get_stride() const
    {
        return stride;
    }
};
