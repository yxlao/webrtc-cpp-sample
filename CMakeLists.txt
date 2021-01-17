cmake_minimum_required(VERSION 3.8)
project(webrtc-cpp CXX)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wno-unused-parameter")

add_executable(sample
  main.cpp
)

# WEBRTC_LINUX, WEBRTC_MAC, WEBRTC_POSIX, WEBRTC_WIN
target_compile_definitions(sample PRIVATE WEBRTC_LINUX=1 WEBRTC_POSIX=1)

set(LIBWEBRTC_PATH webrtc)
target_include_directories(sample PRIVATE SYSTEM
    webrtc/include
    webrtc/include/third_party/abseil-cpp
    websocketpp/websocketpp
    picojson
)
target_link_directories(sample PRIVATE webrtc/lib)
target_link_libraries(sample PRIVATE webrtc pthread)
