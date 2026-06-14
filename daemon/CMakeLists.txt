add_executable(daemon src/main.cpp src/bsocket.cpp src/hypr.cpp)
target_link_libraries(daemon PRIVATE core)
target_include_directories(daemon PRIVATE include)
