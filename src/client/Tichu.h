#ifndef TICHUUI_TICHU_H
#define TICHUUI_TICHU_H

#include "sockpp/tcp_connector.h"

#undef ERROR

#include "GUI/Application.h"
#include "panels.h"
#include "../common/serialization/serializable.h"
#include "../common/network/server_msg.h"
#include <unordered_set>
#include <string>
#include <utility>
#include <queue>
#include <chrono>

template<typename T>
class MessageQueue {
public:

    void push(T item) {
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.push(item);
    }

    std::optional<T> try_pop() {
        std::unique_lock<std::mutex> lock(_mutex);

        if (_queue.empty()) {
            return {};
        } else {
            T res = _queue.back();
            _queue.pop();
            return res;
        }
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
};

// use bitflags for state, because we can have multiple states at once
enum class PanelState {
    CONNECTION,
    GAME,
};

class TichuGame : public Layer {
public:

    void on_attach() override;

    void on_detach() override;

    void on_update(TimeStep ts) override;

    void on_imgui() override;

    void show_msg(MessageType typ, const std::string &msg) {
        // also log message
        switch (typ) {
            case MessageType::ERROR:
                ERROR("{}", msg);
                break;
            case MessageType::WARN:
                WARN("{}", msg);
                break;
            case MessageType::INFO:
                INFO("{}", msg);
                break;
        }
        _messages.emplace_back(typ, msg);
    }

    TichuGame() = default;

    TichuGame(const TichuGame &) = delete;

private:

    void connect_to_server();

    void process_messages();
    void process(const request_response &data);
    void process(const full_state_response &data);

    //PanelState _state{PanelState::CONNECTION};
    PanelState _state{PanelState::GAME};

    // input from the ConnectionPanel
    ConnectionPanel::ConnectionData _connection_data{};

    sockpp::tcp_connector _connection{};
    std::thread _listener{};
    MessageQueue<server_msg> _server_msgs{};

    // all message boxes currently shown. item should be removed if close field is true
    std::vector<Message> _messages{};
};

#endif //TICHUUI_TICHU_H
