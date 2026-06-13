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
    Hypr hypr([&state](Core::Hypr &h) {
        state.hypr = h;
        state.version++;
        std::cout << "active_window: " << h.active_window << "\n"
                  << "active_wsid: " << h.active_wsid << "\n"
                  << "ws_count: " << h.ws_count << "\n";
        for (int i = 0; i < h.ws_count; i++) {
            std::cout << "  ws[" << i << "]: " << h.wss[i].id << " "
                      << h.wss[i].name << "\n";
        }
    });

    while (running) {
        // ...
        bsock.listenb();
    }

    return 0;
}
