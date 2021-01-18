#include <json/json.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

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
            std::cout << "========== Sending ICE begin ==========" << std::endl;
            std::cout << ice.ToJsonString();
            std::cout << "========== Sending ICE end ============" << std::endl;
            ws_server_.send(ws_hdl_, ice.ToJsonString(),
                            websocketpp::frame::opcode::text);
        });
        rtc_manager_.on_message([&](const std::string& message) {
            std::cout << "[RTCServer::on_message] " << message << std::endl;
            if (message == "exit") {
                std::cout << "got message == exit, exiting..." << std::endl;
                // TODO: Use this as a proxy to stop the main event loop. Will
                // be replaced in the future.
                ws_server_.stop_listening();
                std::cout << "!!!!!!!!!!!!!!!!new debug 1" << std::endl;
            } else {
                std::cout << "========= Receive message begin ========="
                          << std::endl;
                std::cout << message << std::endl;
                std::cout << "========= Receive message end ==========="
                          << std::endl;
                std::string reply_message = "Echo of: " + message;
                std::cout << "========= Send message begin ========="
                          << std::endl;
                std::cout << reply_message << std::endl;
                std::cout << "========= Send message end ==========="
                          << std::endl;
                rtc_manager_.send(reply_message);
            }
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
        // DataChannel created. WebSocketClientManager exits its blocking state.
        rtc_manager_.on_success([&]() {
            std::cout << "[RTCServer::on_success]" << std::endl;
            ws_server_.pause_reading(ws_hdl_);
            ws_server_.close(ws_hdl_, websocketpp::close::status::normal, "");
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

        const Json::Value json = StringToJson(message);
        const std::string type = json.get("type", "").asString();
        std::cout << "Received message type: " << type << std::endl;
        std::cout << "Received message: " << message << std::endl;

        if (type == "offer") {
            const std::string offer = json.get("offer", "").asString();
            std::cout << "========== Offer SDP begin ==========" << std::endl;
            std::cout << offer;
            std::cout << "========== Offer SDP end ============" << std::endl;
            rtc_manager_.create_answer_sdp(offer);
        } else if (type == "ice") {
            Ice ice = Ice::FromJsonString(message);
            rtc_manager_.push_ice(ice);
            std::cout << "========== Receive ICE begin ==========" << std::endl;
            std::cout << ice.ToJsonString();
            std::cout << "========== Receive ICE end ============" << std::endl;
        } else {
            throw std::runtime_error("Unkown json message type: " + type);
        }
    }

public:
    /// WebRTCManger.
    WebRTCManager rtc_manager_;

private:
    uint16_t port_;
    WebSocketServer ws_server_;

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

    ws_server_manager.rtc_manager_.quit();
    std::cout << "Server exits gracefully." << std::endl;
    return 0;
}
