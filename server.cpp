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
    WebSocketServerManager(uint16_t port)
        : port_(port), ws_server_(), rtc_manager_("server") {
        rtc_manager_.on_ice([&](const Ice& ice) {
            std::cout << "[RTCServer::on_ice] " << std::endl;
            ice_list_.push_back(ice);
        });
        rtc_manager_.on_message([&](const std::string& message) {
            std::cout << "[RTCServer::on_message] " << message << std::endl;
        });
        rtc_manager_.on_sdp([&](const std::string& sdp) {
            std::cout << "[RTCServer::on_sdp] " << std::endl;
            std::cout << "========== Answer SDP begin ==========" << std::endl;
            std::cout << sdp;
            std::cout << "========== Answer SDP end ============" << std::endl;
            Json::Value json;
            json["type"] = "answer";
            json["answer"] = sdp;
            ws_server_.send(ws_hdl_, JsonToString(json),
                            websocketpp::frame::opcode::text);
        });
        rtc_manager_.init();

        ws_server_.set_open_handler(bind(&WebSocketServerManager::OpenHandler,
                                         this, &ws_server_, _1));
        ws_server_.set_close_handler(bind(&WebSocketServerManager::CloseHandler,
                                          this, &ws_server_, _1));
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
        ws_hdl_ = hdl;
        std::cout << "[WebSocketServerManager::OpenHandler]" << std::endl;
    }

    void CloseHandler(WebSocketServer* ws_server,
                      websocketpp::connection_hdl hdl) {
        ws_hdl_ = hdl;
        std::cout << "[WebSocketServerManager::CloseHandler]" << std::endl;
    }

    void MessageHandler(WebSocketServer* ws_server,
                        websocketpp::connection_hdl hdl,
                        WebSocketServer::message_ptr message_ptr) {
        ws_hdl_ = hdl;
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
            rtc_manager_.create_answer_sdp(offer);
        } else {
            std::cerr << "Unkown json message type: " << type << std::endl;
        }
    }

private:
    uint16_t port_;
    WebSocketServer ws_server_;

    /// WebRTCManger.
    WebRTCManager rtc_manager_;

    /// WebSocket connection handler uniquely identifies a WebSocket connection.
    /// Whenever there is a WebSocket callback, we update this handler. In our
    /// use case, they shall all be the same one.
    websocketpp::connection_hdl ws_hdl_;

    /// List of ICE candidates. It updates when rtc_manager_.on_ice() is
    /// triggered. The server and client exchange their ICE candidate list and
    /// select a candidate to set up the connection.
    std::list<Ice> ice_list_;
};

int main() {
    // TODO: add try-catch for WS connection.
    WebSocketServerManager ws_server_manager(8888);
    return 0;
}
