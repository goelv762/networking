cmake_minimum_required(VERSION 4.2)
project(engine)
cmake_policy(SET CMP0072 NEW)

set(C_EXEC client)
set(S_EXEC server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(BASEPATH "${CMAKE_SOURCE_DIR}")

file(GLOB_RECURSE UI src/ui/*.cpp)

add_library(ui SHARED ${UI})
set_target_properties(ui PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")

add_executable(${C_EXEC} src/client.cpp)
add_executable(${S_EXEC} src/server.cpp)

target_link_libraries(${C_EXEC} ui ncursesw)
