add_library(core STATIC
    src/wayland-context.cpp
    src/surface.cpp
    src/shm-buffer.cpp
    src/renderer.cpp
	src/client.cpp
	src/input-context.cpp
	src/wlr-layer-shell-unstable-v1-client-protocol.c
    src/xdg-shell-client-protocol.c
)

target_include_directories(core PUBLIC include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND REQUIRED wayland-client)
pkg_check_modules(CAIRO REQUIRED cairo)

target_include_directories(core PUBLIC
    ${WAYLAND_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
)

target_link_libraries(core PUBLIC
    ${WAYLAND_LIBRARIES}
    ${CAIRO_LIBRARIES}
)
