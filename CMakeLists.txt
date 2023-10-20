cmake_minimum_required(VERSION 3.22.1)
project(cpp_starter VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(
        ${PROJECT_NAME}
        src/main.cpp
)