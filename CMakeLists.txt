cmake_minimum_required(VERSION 3.8)
project(webrtc-cpp CXX)

set(CMAKE_CXX_STANDARD 14)

# Jsoncpp dependency.
include(jsoncpp.cmake)

# Global flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wno-unused-parameter -std=c++11")

# Global target properties.
function(set_global_target_properties target)
    # WEBRTC_LINUX, WEBRTC_MAC, WEBRTC_POSIX, WEBRTC_WIN.
    target_compile_definitions(sample PRIVATE WEBRTC_LINUX=1 WEBRTC_POSIX=1)

    # Include.
    target_include_directories(${target} PRIVATE SYSTEM
        webrtc/include
        webrtc/include/third_party/abseil-cpp
        websocketpp
        picojson
        asio/asio/include
        ${JSONCPP_INCLUDE_DIRS}
    )

    # Link.
    target_link_directories(${target} PRIVATE
        webrtc/lib
        ${JSONCPP_LIB_DIR}
    )
    target_link_libraries(${target} PRIVATE
        webrtc
        pthread
        ${JSONCPP_LIBRARIES}
    )

    # Dependency.
    add_dependencies(${target} ext_jsoncpp)
endfunction()

add_executable(sample main.cpp)
set_global_target_properties(sample)

add_executable(server server.cpp)
set_global_target_properties(server)

add_executable(client client.cpp)
set_global_target_properties(client)
