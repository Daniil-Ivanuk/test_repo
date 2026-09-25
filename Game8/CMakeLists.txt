cmake_minimum_required(VERSION 3.15)
project(Game8 CXX)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(Game8
    src/main.cpp
    src/game8.cpp
    src/interface.cpp
    src/ai.cpp
)
