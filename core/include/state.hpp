#pragma once

#include <time.h>

#include <cstdint>

#define SHM_PATH "/dev/shm/barbariska"
#define MAX_HYPR_WS_AMOUNT 10

namespace Core {
enum ErrorCode { UNKNOWN };

struct HyprWorkspace {
    int id;
    char name[64];
};

struct Hypr {
    int ws_count;
    int active_wsid;
    HyprWorkspace wss[MAX_HYPR_WS_AMOUNT];
    char active_window[108];
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

}  // namespace Core
