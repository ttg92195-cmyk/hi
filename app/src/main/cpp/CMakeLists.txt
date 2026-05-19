cmake_minimum_required(VERSION 3.22.1)

project("racinggame")

add_library(
    racinggame
    SHARED
    jni_bridge.cpp
)

find_library(log-lib log)

target_link_libraries(racinggame ${log-lib})

set_target_properties(racinggame PROPERTIES CXX_STANDARD 11)
