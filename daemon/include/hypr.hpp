#pragma once

#include <sys/un.h>
#include <unistd.h>

#include <functional>

#include "state.hpp"

enum HyprGetCommand {
    GET_ACTIVE_WORKSPACE,
    GET_WORKSPACES,
    GET_ACTIVE_WINDOW,
};

enum HyprDispatchCommand {
    CHANGE_WORKSPACE,
};

class Hypr {
private:
    /// storing ref to partial state
    Core::Hypr &state;
    /// trigger cb to update version
    std::function<void()> triggerv;

    int fd;
    /// socket1 for requests
    struct sockaddr_un addr;
    /// socket2 events
    struct sockaddr_un addr2;

    /// socket manipulation
    void init_socket();
    void command(HyprGetCommand cmd, char jsonstr[2048]);
    void dispatch_workspace_change(int id);

    /// callbacks for handling hypr state update
    void update_active_window(char w[108]);
    void update_active_workspace(int id);
    void create_workspace(int id);
    void destroy_workspace(int id);

    /// initialize hypr state. Should be called explicitely
    void init_hypr_state();

public:
    Hypr(Core::Hypr &state, std::function<void()> triggerv);
    ~Hypr();

    /// should be placed in backend loop
    void poll_events();
    /// commands to hypr socket
    void command(HyprDispatchCommand cmd, int id);

    int get_fd() const
    {
        return fd;
    }
};
