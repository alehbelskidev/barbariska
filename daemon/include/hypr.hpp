#pragma once

#include <sys/un.h>
#include <unistd.h>

#include <functional>

#include "state.hpp"

// "/dispatch workspace %d"
// j/activewindow
// j/workspaces
// j/activeworkspace

using UpdateHyprStateFn = std::function<void(Core::Hypr &)>;

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
    int fd;
    UpdateHyprStateFn on_update;
    struct sockaddr_un addr;
    struct sockaddr_un addr2;

    void init_socket();
    void command(HyprGetCommand cmd, char jsonstr[2048]);
    void dispatch_workspace_change(int id);

public:
    Hypr(UpdateHyprStateFn update_cb);
    ~Hypr();

    void listen();
    void command(HyprDispatchCommand cmd, int id);
    void init_hypr_state();
};
