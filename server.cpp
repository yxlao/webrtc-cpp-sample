#include <iostream>

#define ASIO_STANDALONE  // Use ASIO standalone lib instead of boost.
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

int main() {
    std::cout << "I am server." << std::endl;
    return 0;
}
