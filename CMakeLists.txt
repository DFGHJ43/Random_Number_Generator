cmake_minimum_required(VERSION 3.10)
project(RandomNumber C)

set(CMAKE_C_STANDARD 99)

add_executable(rng-tui
    src/tui.c
    src/random.c
    src/stats.c
    src/output.c
)

target_link_libraries(rng-tui m)
