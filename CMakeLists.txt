cmake_minimum_required(VERSION 3.8)
project(webrtc-cpp CXX)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wno-unused-parameter")

add_executable(sample
  main.cpp
)

# define WEBRTC_LINUX
# define WEBRTC_MAC
# define WEBRTC_POSIX
# define WEBRTC_WIN
target_compile_definitions(sample PRIVATE WEBRTC_LINUX=1 WEBRTC_POSIX=1)

set(LIBWEBRTC_PATH webrtc)
target_include_directories(sample PRIVATE SYSTEM
    ${LIBWEBRTC_PATH}/include
    ${LIBWEBRTC_PATH}/include/third_party/abseil-cpp
    picojson
)
target_link_directories(sample PRIVATE ${LIBWEBRTC_PATH}/lib)
target_link_libraries(sample PRIVATE webrtc pthread)
