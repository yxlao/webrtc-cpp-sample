#include <iostream>
#include <string>

#define ASIO_STANDALONE  // Use ASIO standalone lib instead of boost.
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> WebSocketClient;

void OnWebSocketMessage(WebSocketClient* ws_client,
                        websocketpp::connection_hdl hdl,
                        WebSocketClient::message_ptr message_ptr) {
    const std::string message = message_ptr->get_payload();
    std::cout << "[Client] Received message: " << message << std::endl;
}

void OnWebSocketOpen(WebSocketClient* ws_client,
                     websocketpp::connection_hdl hdl) {
    std::cout << "[Client::OnWebSocketOpen]" << std::endl;
    ws_client->send(hdl, "Hello from Client.",
                    websocketpp::frame::opcode::text);
}

void OnWebSocketClose(WebSocketClient* ws_client,
                      websocketpp::connection_hdl hdl) {
    std::cout << "[Client::OnWebSocketClose]" << std::endl;
}

int main() {
    using websocketpp::lib::bind;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;

    std::cout << "[Client] I am the client." << std::endl;

    // TODO: Add close connection.
    WebSocketClient ws_client;
    ws_client.init_asio();
    ws_client.set_open_handler(bind(OnWebSocketOpen, &ws_client, _1));
    ws_client.set_close_handler(bind(OnWebSocketOpen, &ws_client, _1));
    ws_client.set_message_handler(bind(OnWebSocketMessage, &ws_client, _1, _2));

    // Create a connection to the given URI and queue it for connection once
    // the event loop starts.
    std::string uri = "ws://localhost:8888";
    websocketpp::lib::error_code ec;
    WebSocketClient::connection_ptr con = ws_client.get_connection(uri, ec);
    ws_client.connect(con);
    ws_client.run();

    std::cout << "[Client] Process ends." << std::endl;

    return 0;
}

// int main() {
//     // Simple client that can send and receive messages.
//     WebRTCClient rtc_client = InitWebRTCClient("ws://localhost:8888");
//     rtc_client.send("Hello world!");
//     rtc_client.send("CLOSE_SERVER");
//     rtc_client.close();
//     return 0;
// }
