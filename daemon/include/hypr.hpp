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
    Core::Hypr &state;
    std::function<void()> triggerv;

    int fd;
    struct sockaddr_un addr;
    struct sockaddr_un addr2;

    void init_socket();
    void command(HyprGetCommand cmd, char jsonstr[2048]);
    void dispatch_workspace_change(int id);

    void update_active_window(char w[108]);
    void update_active_workspace(int id);
    void create_workspace(int id);
    void destroy_workspace(int id);

public:
    Hypr(Core::Hypr &state, std::function<void()> triggerv);
    ~Hypr();

    void listen();
    void command(HyprDispatchCommand cmd, int id);
    void init_hypr_state();
};
