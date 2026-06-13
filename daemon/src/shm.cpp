#include "shm.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <iostream>

Shm::Shm()
{
    fd = open(BARBARISKA_SHM_PATH, O_RDWR | O_CREAT, 0600);
    if (fd == -1) {
        std::cerr << "DERROR: Failed to create barbariska shared mem region\n";
        return;
    }
    ftruncate(fd, sizeof(Core::State));

    void *res =
        mmap(nullptr, sizeof(Core::State), PROT_WRITE, MAP_SHARED, fd, 0);
    if (res == MAP_FAILED) {
        std::cerr << "DERROR: Failed to map barbariska shared mem region\n";
        shm_ptr = nullptr;
    } else {
        shm_ptr = static_cast<Core::State *>(res);
    }
}

Shm::~Shm()
{
    munmap(shm_ptr, sizeof(Core::State));
    close(fd);
}

void Shm::write(const Core::State &state)
{
    *shm_ptr = state;
}
