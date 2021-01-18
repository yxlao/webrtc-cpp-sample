#include <iostream>
#include <string>

#define ASIO_STANDALONE  // Use ASIO standalone lib instead of boost.
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef websocketpp::server<websocketpp::config::asio> WebSocketServer;

void OnWebSocketMessage(WebSocketServer* ws_server,
                        websocketpp::connection_hdl hdl,
                        WebSocketServer::message_ptr message_ptr) {
    const std::string message = message_ptr->get_payload();
    std::cout << "[Server::OnWebSocketMessage] Received message: " << message
              << std::endl;

    if (message == "close_server") {
        ws_server->close(hdl, websocketpp::close::status::normal, "");
        std::cout << "[Server] Server WebSocket closed." << std::endl;
    }
}

void OnWebSocketOpen(WebSocketServer* ws_server,
                     websocketpp::connection_hdl hdl) {
    std::cout << "[Server::OnWebSocketOpen]" << std::endl;
    ws_server->send(hdl, "Hello from server.",
                    websocketpp::frame::opcode::text);
}

void OnWebSocketClose(WebSocketServer* ws_server) {
    std::cout << "[Server::OnWebSocketClose]" << std::endl;
}

int main() {
    using websocketpp::lib::bind;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;

    std::cout << "[Server] I am the server." << std::endl;

    // TODO: Add close connection.
    WebSocketServer ws_server;
    ws_server.set_message_handler(bind(OnWebSocketMessage, &ws_server, _1, _2));
    ws_server.set_open_handler(bind(OnWebSocketOpen, &ws_server, _1));
    ws_server.set_close_handler(bind(OnWebSocketClose, &ws_server));
    ws_server.init_asio();
    ws_server.set_reuse_addr(true);
    ws_server.listen(8888);
    ws_server.start_accept();
    ws_server.run();

    std::cout << "[Server] Process ends." << std::endl;

    return 0;
}

// TODO: goal
// int main() {
//     // Simple echo server that receives client's message and reply back.
//     WebRTCServer rtc_server = InitWebRTCServer("8888");
//     rtc_server.run();
//     return 0;
// }
