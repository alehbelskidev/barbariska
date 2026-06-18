#include "shm-buffer.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

core::ShmBuffer::ShmBuffer(Surface::Dimensions surface_dimensions, wl_shm *shm)
{
    stride = surface_dimensions.bar_width * 4;
    shm_size = stride * surface_dimensions.bar_height;

    // shm_open creates anon file in RAM
    // COMPOSITOR and our process share this memory region
    int fd = shm_open("/barbariska-bar", O_RDWR | O_CREAT, 0600);
    shm_unlink("/barbariska-bar");  // delete name , keep file descriptor alive
    ftruncate(fd, shm_size);        // set filesize to fit pixels

    shm_data =
        mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // wl_shm_pool wraps our fd for COMPOSITOR
    // COMPOSITOR now knows about this memory region
    wl_shm_pool *pool = wl_shm_create_pool(shm, fd, shm_size);

    buffer = wl_shm_pool_create_buffer(pool, 0, surface_dimensions.bar_width,
                                       surface_dimensions.bar_height, stride,
                                       WL_SHM_FORMAT_ARGB8888);

    // pool is useless after creating buffer
    wl_shm_pool_destroy(pool);
    // fd as well, mmap keeps memory
    close(fd);
}

core::ShmBuffer::~ShmBuffer()
{
    munmap(shm_data, shm_size);
    wl_buffer_destroy(buffer);
}
