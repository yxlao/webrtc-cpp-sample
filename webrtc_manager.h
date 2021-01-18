#pragma once

#include <iostream>

#define ASIO_STANDALONE  // Use ASIO standalone lib instead of boost.
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

class WebRTCManager {
public:
    WebRTCManager() {}
    virtual ~WebRTCManager() {}

    void Send(const std::string& message) {
        std::cout << "WebRTCManager::Send " << message << std::endl;
    }
};
