cmake_minimum_required(VERSION 2.4.3)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
cmake_policy(SET CMP0017 NEW)
SET(MSVC_INCREMENTAL_DEFAULT ON)

PROJECT(visual_debugger_shared_memory)

# add_executable(shared_memory_client shared_memory_client.cpp win32_shared_memory.cpp win32_shared_memory.h)
# add_executable(shared_memory_server shared_memory_server.cpp win32_shared_memory.cpp win32_shared_memory.h)

subdirs(src/opengl_window)
subdirs(src/shared_memory)
subdirs(src/dear_imgui)
subdirs(src/platform_utils)

add_executable(shared_memory_client shared_memory_client.cpp)
target_link_libraries(shared_memory_client shared_memory)

add_executable(shared_memory_server shared_memory_server.cpp)
target_link_libraries(shared_memory_server shared_memory)

add_executable(opengl_example opengl_example.cpp )
target_link_libraries(opengl_example opengl_window ${CMAKE_DL_LIBS})

add_executable(opengl_visual_debugger opengl_visual_debugger.cpp src/dear_imgui/opengl3/imgui_impl_opengl3.cpp)
target_link_libraries(opengl_visual_debugger opengl_window dear_imgui shared_memory platform_utils ${CMAKE_DL_LIBS})

