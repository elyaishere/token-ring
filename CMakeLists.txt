cmake_minimum_required(VERSION 3.25)

project(token-ring)

set(CMAKE_CXX_STANDARD             20)
set(CMAKE_EXPORT_COMPILE_COMMANDS  ON)
set(CMAKE_CXX_FLAGS "-g -fsanitize=address,undefined,leak -fno-sanitize-recover=all")

add_subdirectory(token_ring)
add_executable(main main.cpp)
target_link_libraries(main libtokenring)

