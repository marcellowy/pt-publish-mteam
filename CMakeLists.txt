cmake_minimum_required(VERSION 3.29)
project(account VERSION 1.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

if (POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif (POLICY CMP0091)

find_package(jsoncpp CONFIG REQUIRED)

add_subdirectory(src)