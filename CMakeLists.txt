cmake_minimum_required(VERSION 3.7)
project(fightclub)

set(CMAKE_C_STANDARD 99)

include_directories(/home/johnny1024/openmpi/include/)

set(SOURCE_FILES main.c myLists.h)
add_executable(fightclub ${SOURCE_FILES})

