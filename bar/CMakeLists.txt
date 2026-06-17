add_executable(bar
	src/main.cpp
	src/config.cpp
	src/bar-renderer.cpp
)
target_include_directories(bar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(bar PRIVATE core)
