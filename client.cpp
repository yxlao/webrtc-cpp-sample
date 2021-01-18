#include <json/json.h>

#include <iostream>
#include <string>

#define ASIO_STANDALONE  // Use ASIO standalone lib instead of boost.
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

class WebSocketClientManager {
    using WebSocketClient =
            websocketpp::client<websocketpp::config::asio_client>;

public:
    WebSocketClientManager(const std::string& uri) : uri_(uri), ws_client_() {
        ws_client_.set_open_handler(bind(OpenHandler, &ws_client_, _1));
        ws_client_.set_close_handler(bind(CloseHandler, &ws_client_, _1));
        ws_client_.set_message_handler(
                bind(MessageHandler, &ws_client_, _1, _2));
        ws_client_.init_asio();
        websocketpp::lib::error_code ec;
        WebSocketClient::connection_ptr connection =
                ws_client_.get_connection(uri, ec);
        ws_client_.connect(connection);
        ws_client_.run();
    }

    virtual ~WebSocketClientManager() {}

    static void OpenHandler(WebSocketClient* ws_client,
                            websocketpp::connection_hdl hdl) {
        std::cout << "[WebSocketClientManager::OpenHandler]" << std::endl;
        ws_client->send(hdl, "Hello from client.",
                        websocketpp::frame::opcode::text);
    }

    static void CloseHandler(WebSocketClient* ws_client,
                             websocketpp::connection_hdl hdl) {
        std::cout << "[WebSocketClientManager::CloseHandler]" << std::endl;
    }

    static void MessageHandler(WebSocketClient* ws_client,
                               websocketpp::connection_hdl hdl,
                               WebSocketClient::message_ptr message_ptr) {
        const std::string message = message_ptr->get_payload();
        std::cout << "[WebSocketClientManager::MessageHandler] Received: "
                  << message << std::endl;
    }

private:
    std::string uri_;
    WebSocketClient ws_client_;
};

int main() {
    WebSocketClientManager ws_client_manager("ws://localhost:8888");
    return 0;
}
