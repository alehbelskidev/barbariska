#pragma once

#include <time.h>

#include <cstdint>

/// For clients to read from daemon
#define BARBARISKA_SOCKET_READ "/run/user/%d/barbariska_read.sock"
/// For daemon to receive notifications from clients
#define BARBARISKA_SOCKET_NOTIF "/run/user/%d/barbariska_notif.sock"

#define MAX_HYPR_WS_AMOUNT 10

namespace core {
enum ErrorCode { UNKNOWN_ERR, NO_ERR };

struct HyprWorkspace {
    int id;
    char name[64];
};

struct Hypr {
    int ws_count;
    int active_wsid;
    HyprWorkspace wss[MAX_HYPR_WS_AMOUNT];
    char active_window_class[108];
    char active_window_title[108];
    ErrorCode error_code;
};

struct Network {
    char essid[33];
    int dmb;
    bool connected;
    ErrorCode error_code;
};

struct State {
    uint64_t version;
    time_t timestamp;

    Network network;
    Hypr hypr;
};

enum CommandT : uint8_t {
    CHANGE_WORKSPACE,
    LAUNCH_APP,
};

struct Command {
    CommandT type;
    char arg[256];
};

}  // namespace core
