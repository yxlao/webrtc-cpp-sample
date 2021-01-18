#include <json/json.h>

#include <iostream>
#include <string>

#include "webrtc_manager.h"

// void OnWebSocketMessage(WebSocketClient* ws_client,
//                         websocketpp::connection_hdl hdl,
//                         WebSocketClient::message_ptr message_ptr) {
//     const std::string message = message_ptr->get_payload();
//     std::cout << "[Client] Received message: " << message << std::endl;
// }

// void OnWebSocketOpen(WebSocketClient* ws_client,
//                      websocketpp::connection_hdl hdl) {
//     std::cout << "[Client::OnWebSocketOpen]" << std::endl;
//     ws_client->send(hdl, "Hello from Client.",
//                     websocketpp::frame::opcode::text);
// }

// void OnWebSocketClose(WebSocketClient* ws_client,
//                       websocketpp::connection_hdl hdl) {
//     std::cout << "[Client::OnWebSocketClose]" << std::endl;
// }

class WebSocketClientManager {
    using WebSocketClient =
            websocketpp::client<websocketpp::config::asio_client>;

public:
    WebSocketClientManager(const std::string& uri) : uri_(uri) {}
    WebRTCManager InitWebRTCManager() { return WebRTCManager(); }

    virtual ~WebSocketClientManager() {}

private:
    std::string uri_;
};

int main() {
    WebSocketClientManager ws_client_manager("ws://localhost:8888");
    WebRTCManager rtc_manager = ws_client_manager.InitWebRTCManager();
    rtc_manager.Send("Hello world from client!");
    return 0;
}

// int main() {
//     using websocketpp::lib::bind;
//     using websocketpp::lib::placeholders::_1;
//     using websocketpp::lib::placeholders::_2;
//
//     std::cout << "[Client] I am the client." << std::endl;
//
//     // TODO: Add close connection.
//     WebSocketClient ws_client;
//     ws_client.init_asio();
//     ws_client.set_open_handler(bind(OnWebSocketOpen, &ws_client, _1));
//     ws_client.set_close_handler(bind(OnWebSocketOpen, &ws_client, _1));
//     ws_client.set_message_handler(bind(OnWebSocketMessage, &ws_client, _1,
//     _2));
//
//     // Create a connection to the given URI and queue it for connection once
//     // the event loop starts.
//     std::string uri = "ws://localhost:8888";
//     websocketpp::lib::error_code ec;
//     WebSocketClient::connection_ptr con = ws_client.get_connection(uri, ec);
//     ws_client.connect(con);
//     ws_client.run();
//
//     std::cout << "[Client] Process ends." << std::endl;
//
//     return 0;
// }
