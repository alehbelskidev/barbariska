add_executable(daemon src/main.cpp src/socket.cpp src/shm.cpp src/hypr.cpp)
target_link_libraries(daemon PRIVATE core)
target_include_directories(daemon PRIVATE include)
