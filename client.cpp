#include <json/json.h>

#include <iostream>
#include <string>

#define ASIO_STANDALONE  // Use ASIO standalone lib instead of boost.
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

#include "webrtc_manager.h"

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

class WebSocketClientManager {
    using WebSocketClient =
            websocketpp::client<websocketpp::config::asio_client>;

public:
    WebSocketClientManager(const std::string& uri)
        : uri_(uri), ws_client_(), rtc_manager_("client") {
        std::list<Ice> ice_list;
        rtc_manager_.on_ice([&](const Ice& ice) {
            std::cout << "[RTCClient::on_ice] " << std::endl;
            // ice_list.push_back(ice);
        });
        rtc_manager_.on_message([&](const std::string& message) {
            std::cout << "[RTCClient::on_message] " << message << std::endl;
        });
        rtc_manager_.on_sdp([&](const std::string& sdp) {
            std::cout << "[RTCClient::on_sdp] " << std::endl;
            std::cout << "========== Offer SDP begin ==========" << std::endl;
            std::cout << sdp;
            std::cout << "========== Offer SDP end ============" << std::endl;
            Json::Value json;
            json["type"] = "offer";
            json["value"] = sdp;
            ws_client_.send(
                    ws_hdl_,
                    Json::writeString(Json::StreamWriterBuilder(), json),
                    websocketpp::frame::opcode::text);
        });
        rtc_manager_.init();

        ws_client_.set_open_handler(bind(&WebSocketClientManager::OpenHandler,
                                         this, &ws_client_, _1));
        ws_client_.set_close_handler(bind(&WebSocketClientManager::CloseHandler,
                                          this, &ws_client_, _1));
        ws_client_.set_message_handler(
                bind(&WebSocketClientManager::MessageHandler, this, &ws_client_,
                     _1, _2));
        ws_client_.init_asio();
        websocketpp::lib::error_code ec;
        WebSocketClient::connection_ptr connection =
                ws_client_.get_connection(uri, ec);
        ws_client_.connect(connection);
        ws_client_.run();
    }

    virtual ~WebSocketClientManager() {}

    // Send WebRTC offer once the WebSocket connection is established.
    void OpenHandler(WebSocketClient* ws_client,
                     websocketpp::connection_hdl hdl) {
        ws_hdl_ = hdl;
        std::cout << "[WebSocketClientManager::OpenHandler]" << std::endl;
        rtc_manager_.create_offer_sdp();  // Triggers rtc_manager_.on_sdp.
    }

    void CloseHandler(WebSocketClient* ws_client,
                      websocketpp::connection_hdl hdl) {
        ws_hdl_ = hdl;
        std::cout << "[WebSocketClientManager::CloseHandler]" << std::endl;
    }

    void MessageHandler(WebSocketClient* ws_client,
                        websocketpp::connection_hdl hdl,
                        WebSocketClient::message_ptr message_ptr) {
        ws_hdl_ = hdl;
        const std::string message = message_ptr->get_payload();
        std::cout << "[WebSocketClientManager::MessageHandler] Received: "
                  << message << std::endl;
    }

private:
    std::string uri_;
    WebSocketClient ws_client_;
    WebRTCManager rtc_manager_;
    /// WebSocket connection handler uniquely identifies a WebSocket connection.
    /// Whenever there is a WebSocket callback, we update this handler. In our
    /// use case, they shall all be the same one.
    websocketpp::connection_hdl ws_hdl_;
};

int main() {
    WebSocketClientManager ws_client_manager("ws://localhost:8888");
    return 0;
}
