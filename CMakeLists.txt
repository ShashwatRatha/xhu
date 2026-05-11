cmake_minimum_required(VERSION 3.10)

project(Xhu VERSION 3.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*.c")
add_executable(xhu ${SOURCES})

target_include_directories(xhu PUBLIC include)
