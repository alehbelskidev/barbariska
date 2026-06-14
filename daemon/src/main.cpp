#include <poll.h>

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
    Shm shm;
    Core::State state;
    Socket sock([](const Core::Command *cmd) {
        std::cout << "Reading from sock example " << cmd->type << "\n";
    });
    Hypr hypr(state.hypr, [&state, &shm]() {
        state.version++;
        shm.write(state);
        std::cout << "SHM write: version=" << state.version
                  << " window=" << state.hypr.active_window << "\n";
    });

    std::cout << "State " << state.hypr.active_window << "\n";

    pollfd fds[2];
    fds[0].fd = sock.get_fd();
    fds[0].events = POLLIN;
    fds[1].fd = hypr.get_fd();
    fds[1].events = POLLIN;

    while (running) {
        poll(fds, 2, -1);

        if (fds[0].revents & POLLIN) sock.poll_events();
        if (fds[1].revents & POLLIN) hypr.poll_events();
    }

    return 0;
}
