find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND REQUIRED wayland-client)
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)
pkg_check_modules(CAIRO REQUIRED cairo)

add_executable(bar
    src/main.cpp
    src/wlr-layer-shell-unstable-v1-client-protocol.c
    src/xdg-shell-client-protocol.c
)

target_include_directories(bar PRIVATE
    ${WAYLAND_INCLUDE_DIRS}
    ${CAIRO_INCLUDE_DIRS}
	include
)
target_link_libraries(bar PRIVATE
    ${WAYLAND_LIBRARIES}
    ${CAIRO_LIBRARIES}
)

set_target_properties(bar PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/barbariska
)
