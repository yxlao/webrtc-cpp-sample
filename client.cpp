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

using WebSocketClient = websocketpp::client<websocketpp::config::asio_client>;
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

class WebSocketClientManager {
public:
    WebSocketClientManager(const std::string& uri) : uri_(uri), ws_client_() {
        ws_client_.init_asio();
        ws_client_.set_open_handler(bind(OnWebSocketOpen, &ws_client_, _1));
    }

    virtual ~WebSocketClientManager() {}

    WebRTCManager InitWebRTCManager() { return WebRTCManager(); }

    static void OnWebSocketOpen(WebSocketClient* ws_client,
                                websocketpp::connection_hdl hdl) {
        std::cout << "[Client::OnWebSocketOpen]" << std::endl;
        ws_client->send(hdl, "Hello from Client.",
                        websocketpp::frame::opcode::text);
    }

private:
    std::string uri_;
    WebSocketClient ws_client_;
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
//     WebSocketClient ws_client_;
//     ws_client_.init_asio();
//     ws_client_.set_open_handler(bind(OnWebSocketOpen, &ws_client_, _1));
//     ws_client_.set_close_handler(bind(OnWebSocketOpen, &ws_client_, _1));
//     ws_client_.set_message_handler(bind(OnWebSocketMessage, &ws_client_, _1,
//     _2));
//
//     // Create a connection to the given URI and queue it for connection once
//     // the event loop starts.
//     std::string uri = "ws://localhost:8888";
//     websocketpp::lib::error_code ec;
//     WebSocketClient::connection_ptr con = ws_client_.get_connection(uri, ec);
//     ws_client_.connect(con);
//     ws_client_.run();
//
//     std::cout << "[Client] Process ends." << std::endl;
//
//     return 0;
// }
