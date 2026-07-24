#include <poll.h>
#include <sys/timerfd.h>

#include <chrono>
#include <csignal>
#include <iostream>

#include "bsocket.hpp"
#include "core.hpp"
#include "hypr.hpp"

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

    core::State state;

    BSocket bsock([](const core::Command &cmd) {
        std::cout << "Reading from sock example " << cmd.type << "\n";
    });
    Hypr hypr(state.hypr, [&state, &bsock]() {
        state.version++;
        bsock.broadcast(state);
    });

    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
    struct itimerspec ts;
    ts.it_interval.tv_sec = 1;
    ts.it_interval.tv_nsec = 0;
    ts.it_value = ts.it_interval;
    timerfd_settime(timerfd, 0, &ts, nullptr);

    pollfd fds[4];
    fds[0].fd = bsock.get_notiffd();
    fds[0].events = POLLIN;
    fds[1].fd = hypr.get_fd();
    fds[1].events = POLLIN;
    fds[2].fd = bsock.get_readfd();
    fds[2].events = POLLIN;
    fds[3].fd = timerfd;
    fds[3].events = POLLIN;

    while (running) {
        poll(fds, 4, -1);

        if (fds[0].revents & POLLIN) bsock.notif_poll_events();
        if (fds[1].revents & POLLIN) hypr.poll_events();
        if (fds[2].revents & POLLIN) bsock.accept_client(state);

        if (fds[3].revents & POLLIN) {
            uint64_t exp;
            read(timerfd, &exp, sizeof(exp));

            state.timestamp = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now());
            state.version += 1;
            bsock.broadcast(state);
        }
    }

    close(timerfd);
    return 0;
}
