#include <iostream>
#include <string>

#define ASIO_STANDALONE  // Use ASIO standalone lib instead of boost.
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> WebSocketServer;
typedef WebSocketServer::message_ptr MessagePtr;

void OnWebSocketMessage(WebSocketServer* ws_server,
                        websocketpp::connection_hdl hdl,
                        MessagePtr message_ptr) {
    const std::string message = message_ptr->get_payload();
    std::cout << "[Server] Received message: " << message << std::endl;

    if (message == "close_server") {
        ws_server->close(hdl, websocketpp::close::status::normal, "");
        std::cout << "[Server] Server WebSocket closed." << std::endl;
    }
}

int main() {
    using websocketpp::lib::bind;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;

    std::cout << "[Server] I am the server." << std::endl;

    // TODO: Add close connection.
    WebSocketServer ws_server;
    ws_server.set_message_handler(bind(OnWebSocketMessage, &ws_server, _1, _2));
    ws_server.init_asio();
    ws_server.set_reuse_addr(true);
    ws_server.listen(8888);
    ws_server.start_accept();
    ws_server.run();

    std::cout << "[Server] Server process ends." << std::endl;

    return 0;
}
