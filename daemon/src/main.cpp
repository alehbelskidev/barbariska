#include <poll.h>

#include <csignal>
#include <iostream>

#include "bsocket.hpp"
#include "hypr.hpp"
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
    signal(SIGPIPE, SIG_IGN);

    Core::State state;

    BSocket bsock([](const Core::Command &cmd) {
        std::cout << "Reading from sock example " << cmd.type << "\n";
    });
    Hypr hypr(state.hypr, [&state, &bsock]() {
        state.version++;
        bsock.broadcast(state);
    });

    pollfd fds[3];
    fds[0].fd = bsock.get_notiffd();
    fds[0].events = POLLIN;
    fds[1].fd = hypr.get_fd();
    fds[1].events = POLLIN;
    fds[2].fd = bsock.get_readfd();
    fds[2].events = POLLIN;

    while (running) {
        poll(fds, 3, -1);

        if (fds[0].revents & POLLIN) bsock.notif_poll_events();
        if (fds[1].revents & POLLIN) hypr.poll_events();
        if (fds[2].revents & POLLIN) bsock.accept_client(state);
    }

    return 0;
}
