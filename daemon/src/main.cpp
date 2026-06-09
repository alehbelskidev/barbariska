#include <csignal>

#include "state.hpp"

static bool running = true;

void on_signal(int)
{
    running = false;
}

int main()
{
    Core::State state;
    signal(SIGTERM, on_signal);
    signal(SIGINT, on_signal);

    while (running) {
        // ...
    }

    return 0;  // деструкторы вызовутся
}
