#include "hypr.hpp"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <iostream>

#include "json.hpp"

using json = nlohmann::json;

void Hypr::init_socket()
{
    fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (connect(fd, (struct sockaddr *)&addr2, sizeof(addr2)) == -1) {
        close(fd);
        fd = -1;
        std::cerr << "Failed to init sock2 listener\n";
        return;
    }

    fcntl(fd, F_SETFL, O_NONBLOCK);
}

void Hypr::update_active_window(char w[108])
{
    if (strcmp(state.active_window, w) != 0) {
        memcpy(state.active_window, w, sizeof(state.active_window) - 1);
        state.active_window[sizeof(state.active_window) - 1] = '\0';
    }
}
void Hypr::update_active_workspace(int id)
{
    if (state.active_wsid != id) {
        state.active_wsid = id;
    }
}
void Hypr::create_workspace(int id)
{
    char name[128];
    snprintf(name, sizeof(name), "%d", id);
    Core::HyprWorkspace new_ws = {id, ""};
    strncpy(new_ws.name, name, sizeof(new_ws.name) - 1);
    new_ws.name[sizeof(new_ws.name) - 1] = '\0';

    int i = state.ws_count - 1;

    while (i >= 0 && state.wss[i].id > id) {
        state.wss[i + 1] = state.wss[i];
        i--;
    }

    state.wss[i + 1] = new_ws;

    state.ws_count++;
}
void Hypr::destroy_workspace(int id)
{
    int write_idx = 0;

    for (int i = 0; i < state.ws_count; i++) {
        if (state.wss[i].id != id) {
            state.wss[write_idx] = state.wss[i];
            write_idx++;
        }
    }

    state.ws_count = write_idx;
}

void Hypr::listen()
{
    size_t bufsize = 4096;
    char buf[bufsize];

    int n = read(fd, buf, bufsize - 1);
    if (n > 0) {
        buf[n] = '\0';

        char *line = buf;
        char *end;

        while ((end = strchr(line, '\n')) != NULL) {
            *end = '\0';

            char event_t[64], arg[108];

            int res_count = sscanf(line, "%63[^>]>>%107[^\n]", event_t, arg);

            if (res_count != 2) {
                line = end + 1;
                continue;
            }

            if (strcmp(event_t, "activewindow") == 0) {
                update_active_window(arg);
                triggerv();
            } else if (strcmp(event_t, "workspace") == 0) {
                update_active_workspace(atoi(arg));
                triggerv();
            } else if (strcmp(event_t, "createworkspace") == 0) {
                create_workspace(atoi(arg));
                triggerv();
            } else if (strcmp(event_t, "destroyworkspace") == 0) {
                destroy_workspace(atoi(arg));
                triggerv();
            }

            line = end + 1;
        }
    }
}

// j/activewindow
// j/workspaces
// j/activeworkspace
void get_get_cmd(HyprGetCommand cmd, char ret_cmd_str[108])
{
    char cmdstr[108];
    switch (cmd) {
        case GET_ACTIVE_WORKSPACE:
            snprintf(ret_cmd_str, 108, "%s", "j/activeworkspace");
            break;
        case GET_WORKSPACES:
            snprintf(ret_cmd_str, 108, "%s", "j/workspaces");
            break;
        case GET_ACTIVE_WINDOW:
            snprintf(ret_cmd_str, 108, "%s", "j/activewindow");
            break;
    }
}

void Hypr::command(HyprGetCommand cmd, char jsonstr[2048])
{
    char cmdstr[108];
    get_get_cmd(cmd, cmdstr);

    int gfd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (connect(gfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        std::cerr << "DERROR: Failed to connect to socket" << "\n";
        std::cerr << "DERROR: Addr sun path: " << addr.sun_path << "\n";
        close(fd);
    }

    write(gfd, cmdstr, strlen(cmdstr));
    int n = read(gfd, jsonstr, 2047);
    jsonstr[n] = '\0';

    close(gfd);
}

void Hypr::dispatch_workspace_change(int id)
{
    char path[256];
    snprintf(path, sizeof(path), "%s/hypr/%s/.socket.sock",
             getenv("XDG_RUNTIME_DIR"), getenv("HYPRLAND_INSTANCE_SIGNATURE"));

    int dfd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (connect(dfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(dfd);
        return;
    }

    char *cmd;
    snprintf(cmd, 64, "/dispatch workspace %d", id);
    write(dfd, cmd, strlen(cmd));
    close(dfd);
}

void Hypr::init_hypr_state()
{
    char active_window_jsonstr[2048];
    char active_workspace_jsonstr[2048];
    char workspaces_jsonstr[2048];

    command(GET_ACTIVE_WINDOW, active_window_jsonstr);
    command(GET_ACTIVE_WORKSPACE, active_workspace_jsonstr);
    command(GET_WORKSPACES, workspaces_jsonstr);

    try {
        auto awin_json = json::parse(active_window_jsonstr);
        snprintf(state.active_window, 108, "%s - %s",
                 awin_json["class"].get<std::string>().c_str(),
                 awin_json["title"].get<std::string>().c_str());

        auto aws_json = json::parse(active_workspace_jsonstr);
        state.active_wsid = aws_json["id"];

        auto wss_json = json::parse(workspaces_jsonstr);
        for (json::iterator it = wss_json.begin(); it != wss_json.end(); ++it) {
            Core::HyprWorkspace ws;
            ws.id = (*it)["id"];
            strcpy(ws.name, (*it)["name"].get<std::string>().c_str());
            state.wss[state.ws_count] = ws;
            state.ws_count++;
        }

        state.error_code = Core::NO_ERR;
    } catch (const json::exception &e) {
        std::cerr << "Invalid JSON: " << e.what() << "\n";
        state.error_code = Core::UNKNOWN_ERR;
    }
}

void Hypr::command(HyprDispatchCommand cmd, int id)
{
    switch (cmd) {
        case CHANGE_WORKSPACE:
            dispatch_workspace_change(id);
            break;
    }
}

Hypr::Hypr(Core::Hypr &state, std::function<void()> triggerv)
    : state(state), triggerv(triggerv)
{
    char path[256];
    snprintf(path, 256, "%s/hypr/%s/.socket.sock", getenv("XDG_RUNTIME_DIR"),
             getenv("HYPRLAND_INSTANCE_SIGNATURE"));

    addr = {};
    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    char path2[256];
    snprintf(path2, 256, "%s/hypr/%s/.socket2.sock", getenv("XDG_RUNTIME_DIR"),
             getenv("HYPRLAND_INSTANCE_SIGNATURE"));

    addr2 = {};
    addr2.sun_family = AF_UNIX;
    memcpy(addr2.sun_path, path2, sizeof(addr2.sun_path) - 1);
    addr2.sun_path[sizeof(addr2.sun_path) - 1] = '\0';

    init_socket();
    init_hypr_state();
}

Hypr::~Hypr()
{
    if (fd > 0) close(fd);
}
