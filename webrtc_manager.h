#pragma once

#include <iostream>

class WebRTCManager {
public:
    WebRTCManager() {}
    virtual ~WebRTCManager() {}

    void Send(const std::string& message) {
        std::cout << "WebRTCManager::Send " << message << std::endl;
    }
};
