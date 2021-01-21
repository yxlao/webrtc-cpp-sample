cmake_minimum_required(VERSION 3.8)
project(webrtc-cpp CXX)

set(CMAKE_CXX_STANDARD 14)

# Jsoncpp dependency.
include(jsoncpp/jsoncpp.cmake)

# Global flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wno-unused-parameter -std=c++11")

# Global target properties.
function(set_global_target_properties target)
    # WEBRTC_LINUX, WEBRTC_MAC, WEBRTC_POSIX, WEBRTC_WIN.
    target_compile_definitions(${target} PRIVATE WEBRTC_LINUX=1 WEBRTC_POSIX=1)

    # Include.
    target_include_directories(${target} PRIVATE SYSTEM
        ${PROJECT_SOURCE_DIR}/webrtc/include
        ${PROJECT_SOURCE_DIR}/webrtc/include/third_party/abseil-cpp
        ${PROJECT_SOURCE_DIR}/websocketpp
        ${PROJECT_SOURCE_DIR}/picojson
        ${PROJECT_SOURCE_DIR}/asio/asio/include
        ${JSONCPP_INCLUDE_DIRS}
    )
    target_include_directories(${target} PRIVATE
        ${PROJECT_SOURCE_DIR}/src
    )

    # Link.
    target_link_directories(${target} PRIVATE
        ${PROJECT_SOURCE_DIR}/webrtc/lib
        ${JSONCPP_LIB_DIR}
    )
    target_link_libraries(${target} PRIVATE
        webrtc
        pthread
        ${JSONCPP_LIBRARIES}
    )

    # Output location.
    set_target_properties(${target}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    # Dependency.
    add_dependencies(${target} ext_jsoncpp)
endfunction()

add_subdirectory(src)


