cmake_minimum_required(VERSION 3.22.1)

project("racing3d")

add_library(
    racing3d
    SHARED
    jni_bridge.cpp
)

find_library(log-lib log)
find_library(gles-lib GLESv2)

target_link_libraries(racing3d ${log-lib} ${gles-lib})

set_target_properties(racing3d PROPERTIES CXX_STANDARD 11)
