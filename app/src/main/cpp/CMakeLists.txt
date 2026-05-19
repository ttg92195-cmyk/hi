cmake_minimum_required(VERSION 3.18.1)

project("racinggame")

# Add C++ game engine source files
add_library(
    racinggame
    SHARED
    game_engine.h
    jni_bridge.cpp
)

# Find Android log library
find_library(log-lib log)

# Link libraries
target_link_libraries(racinggame ${log-lib})

# C++ standard
set_target_properties(racinggame PROPERTIES CXX_STANDARD 11)
