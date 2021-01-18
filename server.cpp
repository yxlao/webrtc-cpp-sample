#include <json/json.h>

#include <iostream>
#include <string>

#define ASIO_STANDALONE  // Use ASIO standalone lib instead of boost.
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "json_utils.h"
#include "webrtc_manager.h"

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

class WebSocketServerManager {
    using WebSocketServer = websocketpp::server<websocketpp::config::asio>;

public:
    WebSocketServerManager(uint16_t port) : port_(port), ws_server_() {
        ws_server_.set_open_handler(bind(&WebSocketServerManager::OpenHandler,
                                         this, &ws_server_, _1));
        ws_server_.set_close_handler(
                bind(&WebSocketServerManager::CloseHandler, this, &ws_server_));
        ws_server_.set_message_handler(
                bind(&WebSocketServerManager::MessageHandler, this, &ws_server_,
                     _1, _2));
        ws_server_.init_asio();
        ws_server_.set_reuse_addr(true);
        ws_server_.listen(port_);
        ws_server_.start_accept();
        ws_server_.run();
    }

    virtual ~WebSocketServerManager() {}

    void OpenHandler(WebSocketServer* ws_server,
                     websocketpp::connection_hdl hdl) {
        std::cout << "[WebSocketServerManager::OpenHandler]" << std::endl;
    }

    void CloseHandler(WebSocketServer* ws_server) {
        std::cout << "[WebSocketServerManager::CloseHandler]" << std::endl;
    }

    void MessageHandler(WebSocketServer* ws_server,
                        websocketpp::connection_hdl hdl,
                        WebSocketServer::message_ptr message_ptr) {
        const std::string message = message_ptr->get_payload();
        std::cout << "[WebSocketServerManager::MessageHandler]" << std::endl;
        std::cout << "Received message: " << message << std::endl;

        const Json::Value json = StringToJson(message);
        const std::string type = json.get("type", "").asString();
        if (type == "offer") {
            const std::string offer = json.get("offer", "").asString();
            std::cout << "========== Offer SDP begin ==========" << std::endl;
            std::cout << offer;
            std::cout << "========== Offer SDP end ============" << std::endl;
        } else {
            std::cerr << "Unkown json message type: " << type << std::endl;
        }
    }

private:
    uint16_t port_;
    WebSocketServer ws_server_;
};

int main() {
    // TODO: add try-catch for WS connection.
    WebSocketServerManager ws_server_manager(8888);
    return 0;
}
