#include <json/json.h>

#include <iostream>
#include <string>

#define ASIO_STANDALONE  // Use ASIO standalone lib instead of boost.
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

#include "json_utils.h"
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
        rtc_manager_.on_ice([&](const Ice& ice) {
            std::cout << "[RTCClient::on_ice] " << std::endl;
            ice_list_.push_back(ice);
            std::cout << "========== Sending ICE begin ==========" << std::endl;
            std::cout << ice.ToJsonString();
            std::cout << "========== Sending ICE end ============" << std::endl;
            ws_client_.send(ws_hdl_, ice.ToJsonString(),
                            websocketpp::frame::opcode::text);
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
            json["offer"] = sdp;
            ws_client_.send(ws_hdl_, JsonToString(json),
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
        std::cout << "[WebSocketClientManager::MessageHandler]" << std::endl;

        const Json::Value json = StringToJson(message);
        const std::string type = json.get("type", "").asString();
        std::cout << "Received message type: " << type << std::endl;
        std::cout << "Received message: " << message << std::endl;

        if (type == "answer") {
            const std::string answer = json.get("answer", "").asString();
            std::cout << "========== Answer SDP begin ==========" << std::endl;
            std::cout << answer;
            std::cout << "========== Answer SDP end ============" << std::endl;
            rtc_manager_.push_reply_sdp(answer);
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

private:
    /// WebSocket server URI that the client will connect to, in the form of
    /// "address:port". The server shall listen to the same port. This is not
    /// the WebRTC server address. WebRTC server address is generated
    /// automatically during WebRTC handshake.
    std::string uri_;

    /// WebSocketpp client is used to send/receive WebRTC handshake (a.k.a.
    /// signaling) messages. WebRTC protocol defines the message formats but
    /// does not take care of sending/receiving handshake messages. The
    /// WebSocket will be closed once the WebRTC connection is established.
    WebSocketClient ws_client_;

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
    WebSocketClientManager ws_client_manager("ws://localhost:8888");
    return 0;
}
