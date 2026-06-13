#include <csignal>
#include <iostream>

#include "hypr.hpp"
#include "shm.hpp"
#include "socket.hpp"
#include "state.hpp"

static bool running = true;

void on_signal(int)
{
    running = false;
}

int main()
{
    signal(SIGTERM, on_signal);
    signal(SIGINT, on_signal);

    Core::State state;
    Shm shm;
    Socket bsock([](const Core::Command *cmd) {
        std::cout << "Reading from sock example " << cmd->type << "\n";
    });
    Hypr hypr(state.hypr, [&state]() { state.version++; });

    while (running) {
        bsock.listenb();
        hypr.listen();
    }

    return 0;
}
