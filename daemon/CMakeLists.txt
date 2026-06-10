add_executable(daemon src/main.cpp src/socket.cpp)
target_link_libraries(daemon PRIVATE core)
target_include_directories(daemon PRIVATE include)
