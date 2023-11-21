#include "Tichu.h"

#include "GUI/imgui_build.h"
#include "misc/cpp/imgui_stdlib.h"
#include "GUI/renderer.h"
#include "sockpp/exception.h"
#include "../../src/common/network/client_msg.h"

bool ConnectionData::validate() {
    if (name.size() > 64 || name.empty()) return false;
    if (host.empty()) return false;
    return true;
}

// centering function for the next window. will only center once
void center_next_window_once() {
    auto pos = Application::get_window_size();
    ImGui::SetNextWindowPos({(float) pos.x / 2.f, (float) pos.y / 2.f}, ImGuiCond_Once, {0.5f, 0.5f});
}

// centering function for an item with a label, e.g Button
void center_next_label(const char *label, float alignment = 0.5f) {
    ImGuiStyle &style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}


// show to label for an input field
void show_input_label(const char *label) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("%s", label);
    ImGui::TableNextColumn();
}

// creates a selectable for the given team
void team_selectable(const char *label, TeamSelection *selection, TeamSelection team, float height) {
    ImGui::TableNextColumn();
    if (ImGui::RoundedSelectable(label, team == *selection, 0, {0, height})) {
        *selection = team;
    }
}

/// displays the connection panel, data is read and written into the input argument
void show_connection_panel(ConnectionData *input) {

    auto height = ImGui::GetFontSize();

    auto style = ImGui::ScopedStyle{};
    style.push_style(ImGuiStyleVar_WindowRounding, 20);
    style.push_style(ImGuiStyleVar_WindowMinSize, {750, height * 18});
    style.push_style(ImGuiStyleVar_CellPadding, {height / 2, height / 2});
    style.push_style(ImGuiStyleVar_WindowPadding, {height, height});
    style.push_style(ImGuiStyleVar_SelectableTextAlign, {0.5, 0.5});

    center_next_window_once();
    ImGui::Begin("Connection", nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking |
                 ImGuiWindowFlags_NoScrollbar);

    ImGui::BeginTable("input fields", 2);
    ImGui::TableSetupColumn("field name", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("field input", ImGuiTableColumnFlags_WidthStretch);

    show_input_label("SERVER ADDRESS    ");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputText("##address", &input->host);

    show_input_label("SERVER PORT    ");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputScalar("##port", ImGuiDataType_U32, &input->port);

    show_input_label("PLAYER NAME    ");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputText("##name", &input->name);

    ImGui::EndTable();

    ImGui::BeginTable("buttons", 3);
    team_selectable("TEAM 1", &input->team, TeamSelection::TEAM_1, height * 2);
    team_selectable("TEAM 2", &input->team, TeamSelection::TEAM_2, height * 2);
    team_selectable("RANDOM", &input->team, TeamSelection::RANDOM, height * 2);

    ImGui::TableNextRow();
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TableNextColumn();
    ImGui::TableNextColumn();
    if (ImGui::ButtonEx("CONNECT", {ImGui::GetContentRegionAvail().x, height * 2})) {
        input->connect = true;
    }
    ImGui::EndTable();

    center_next_label(input->status.c_str());
    ImGui::TextColored(ImGui::LIGHT_GREY, "%s", input->status.c_str());

    ImGui::End();
}

/// shows the framebuffer in the viewport window
void show_main_framebuffer() {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::DARK_GREY);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

    ImGui::Begin("viewport");
    auto size = Application::get_viewport_size();
    ImGui::Image(Renderer::get_frame_buffer().get_attachment(0), {(float) size.x, (float) size.y});
    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void TichuGame::on_attach() {
    sockpp::socket_initializer::initialize();
    texture = Texture::load("assets/tichu_logo.png");
}

void TichuGame::on_update(TimeStep ts) {
    float ar = Application::get_aspect_ratio();
    int scale = 5;
    if (ar >= 1) {
        Renderer::set_camera(-(scale * ar), scale * ar, -scale, scale);
    } else {
        ar = 1 / ar;
        Renderer::set_camera(-scale, scale, -(scale * ar), scale * ar);
    }
    Renderer::clear(RGBA{255});

    for (int i = -scale; i < scale; i++) {
        for (int j = -scale; j < scale; j++) {
            if ((i + j) % 2) {
                Renderer::draw_rect({i, j}, {1, 1}, texture);
            }
        }
    }
}

void TichuGame::show_message_boxes() {
    for (auto &messages: _messages) {
        messages.on_imgui();
    }

    // remove closed messages
    auto end = _messages.end();
    auto begin = _messages.begin();
    _messages.erase(std::remove_if(begin, end, [](const MessageWindow &msg) { return msg.should_close; }), end);

}

void TichuGame::on_imgui() {
    // always show available messages
    show_message_boxes();

    if (_state & CONNECTION_PANEL) {
        show_connection_panel(&_connection_data);
    }

    if (_state & GAME_PANEL) {
        show_main_framebuffer();
    }


    if (_connection_data.connect) {
        // reset button press
        _connection_data.connect = false;

        if (_connection_data.validate()) {
            connect_to_server();
        } else {
            show_msg(MessageType::WARN, "invalid input");
        }
    }

}

void listen_to_response(ConnectionData &data, MessageQueue<std::string> *queue) {
    auto join_game = join_game_req {.player_name = "test_name"};
    auto client_req = client_msg(data.id, UUID(), join_game).to_json();
    auto msg = json_utils::to_string(*client_req);
    msg = std::to_string(msg.size()) + ":" + msg;
    data.connection.write(msg);
    char buffer[512];
   while (true) {
       ssize_t count = data.connection.read(buffer, sizeof(buffer));
       if (count <= 0) break;

        INFO_LOG("received: {}", buffer);
   }

}

void TichuGame::connect_to_server() {
    sockpp::inet_address address;

    if (_connection_data.connection.is_connected()) {
        WARN_LOG("connect_to_server was called while already connected!");
        _connection_data.connection.shutdown();
        _listener.join();
    }

    try {
        address = sockpp::inet_address(_connection_data.host, _connection_data.port);
    } catch (const sockpp::getaddrinfo_error& e) {
        show_msg(MessageType::ERROR, "Failed to resolve address " + e.hostname());
        return;
    }

    if (!_connection_data.connection.connect(address)) {
        show_msg(MessageType::ERROR, "Failed to connect to server " + address.to_string());
        return;
    }

    _connection_data.status = "Connected to " + address.to_string();

    try {
        _listener = std::thread(listen_to_response, std::ref(_connection_data), &_server_msgs);
    } catch (std::exception &e) {
        ERROR_LOG("while creating listener thread: {}", e.what());
    }
}

void TichuGame::on_detach() {
    if (_connection_data.connection.is_connected()) {
        _connection_data.connection.shutdown();
        _listener.join();
    }
}

std::pair<std::string, ImVec4> msg_type_to_string_and_color(MessageType typ) {
    switch (typ) {
        case MessageType::ERROR:
            return { "Error", ImGui::RED };
        case MessageType::WARN:
            return { "Warning", ImGui::ORANGE };
        case MessageType::INFO:
            return { "Info", ImGui::BLACK };
    }
    return { "unknown", ImGui::BLACK };
}

void text_wrapped_centered(const std::string& text)
{
    float win_width = ImGui::GetWindowSize().x;
    float text_width = ImGui::CalcTextSize(text.c_str()).x;

    float text_indentation = (win_width - text_width) * 0.5f;

    float min_indentation = 20.0f;
    if (text_indentation <= min_indentation) {
        text_indentation = min_indentation;
    }

    ImGui::SameLine(text_indentation);
    ImGui::PushTextWrapPos(win_width - text_indentation);
    ImGui::TextWrapped("%s", text.c_str());
    ImGui::PopTextWrapPos();
}

void MessageWindow::on_imgui() {
    auto [title, title_color] = msg_type_to_string_and_color(type);

    // add imgui unique id to the title (view docs for more info)
    title += "###" + std::to_string(id);

    auto width = 700.f;
    auto style = ImGui::ScopedStyle{};
    style.push_style(ImGuiStyleVar_WindowMinSize, {width, 0});
    style.push_color(ImGuiCol_TitleBg, title_color);
    style.push_color(ImGuiCol_TitleBgActive, title_color);
    style.push_color(ImGuiCol_Button, ImGui::DARK_GREY);
    style.push_style(ImGuiStyleVar_WindowRounding, 20);
    style.push_style(ImGuiStyleVar_WindowPadding, {20, 20});
    style.push_style(ImGuiStyleVar_WindowTitleAlign, {0.5f, 0.5f});

    center_next_window_once();
    ImGui::Begin(title.c_str(), nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

    text_wrapped_centered(message);

    ImGui::NewLine();

    if (ImGui::ButtonEx("close", {ImGui::GetContentRegionAvail().x, 0})) {
        should_close = true;
    }
    ImGui::End();
}
