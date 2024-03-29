#ifndef TICHUUI_TICHU_H
#define TICHUUI_TICHU_H

#include "sockpp/tcp_connector.h"
#include "Renderer/application.h"
#include "panels.h"
#include "../common/utils.h"
#include "../common/event.h"
#include "../common/messages.h"
#include <unordered_set>
#include <string>
#include <utility>
#include <queue>
#include <chrono>

enum class PanelState {
    CONNECTION,
    GAME,
};


/*! \class TichuGame
    \brief Represents the Tichu game application.
    
 The TichuGame class is responsible for managing the Tichu game application.
 It handles connections to the server, processes messages, and manages the user interface.
*/

class TichuGame : public Layer {
public:

    void on_attach() override;

    void on_detach() override;

    void on_update(TimeStep ts) override;

    void show_msg(MessageType typ, const std::string &msg) {
        // also log message
        switch (typ) {
            case MessageType::Error:
                ERROR("{}", msg);
                break;
            case MessageType::Warn:
                WARN("{}", msg);
                break;
            case MessageType::Info:
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
    void process(const server_message &data);
    void process(const dragon &data);
    void process(const full_state_response &data);

    // show the active panel
    void show();
    // handle the data provided by the user from the gui
    void handle_gui_output();
    void send_message(const ClientMsg &msg);

    PanelState _state{PanelState::CONNECTION};

    // output from the ConnectionPanel
    ConnectionPanel::Data _connection_data{};
    GamePanel::Data _game_panel_data;

    bool _auto_connect{false};
    sockpp::tcp_connector _connection{};
    std::thread _listener{};
    MessageQueue<ServerMsg> _server_msgs{};

    // all message boxes currently shown. item should be removed if close field is true
    std::vector<Message> _messages{};
};

#endif //TICHUUI_TICHU_H
