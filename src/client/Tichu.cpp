#include "Tichu.h"

#include "GUI/imgui_build.h"
#include "misc/cpp/imgui_stdlib.h"
#include "GUI/renderer.h"

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
void show_connection_panel(ConnectionPanelInput *input) {

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
    ImGui::SetWindowFontScale(1.3);

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

void TichuGame::on_update(TimeStep ts) {
    Renderer::set_camera(0, 10, 0, 10);

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if ((i + j) % 2) {
                Renderer::draw_rect({i, j}, {1, 1});
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
    _messages.erase(std::remove_if(begin, end, [](const MessageBox &msg) { return msg.should_close; }), end);

}

void TichuGame::on_imgui() {
    // always show available messages
    show_message_boxes();

    if (_state & Panel::CONNECTION_PANEL) {
        show_connection_panel(&_connection_input);
    }

    if (_state & Panel::GAME_PANEL) {
        show_main_framebuffer();

    }

    if (_connection_input.connect) {
        _state = Panel::GAME_PANEL;
    }

    ImGui::Begin("Debug popup");
    if (ImGui::Button("info")) {
        _messages.emplace_back(MessageType::INFO, "this is an info popup, that informs the player");
    }
    if (ImGui::Button("warn")) {
        _messages.emplace_back(MessageType::WARN, "this is a warning popup, that warns the player");
    }
    if (ImGui::Button("error")) {
        _messages.emplace_back(MessageType::ERROR, "this is an error popup, that tells the player something went wrong");
    }
    ImGui::End();
}

void TichuGame::on_attach() {
}

std::pair<std::string, ImVec4> msg_type_to_string_and_color(MessageType typ) {
    switch (typ) {
        case MessageType::ERROR:
            return { "error", ImGui::RED };
        case MessageType::WARN:
            return { "warning", ImGui::ORANGE };
        case MessageType::INFO:
            return { "info", ImGui::BLACK };
    }
    return { "unknown", ImGui::BLACK };
}

void MessageBox::on_imgui() {
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
    ImGui::SetWindowFontScale(1.3f);
    ImGui::TextWrapped("%s", message.c_str());

    ImGui::NewLine();

    if (ImGui::ButtonEx("close", {ImGui::GetContentRegionAvail().x, 0})) {
        should_close = true;
    }
    ImGui::End();
}
