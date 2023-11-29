#include "panels.h"

#include "Renderer/Application.h"
#include "Renderer/imgui_build.h"
#include "misc/cpp/imgui_stdlib.h"
#include "Renderer/renderer.h"

namespace ImGuiUtils {

// centering function for the next window. will only center once
    void center_next_window_once() {
        auto size = Application::get_window_size();
        ImGui::SetNextWindowPos({(float) size.x / 2.f, (float) size.y / 2.f}, ImGuiCond_Once, {0.5f, 0.5f});
    }

// centering function for an item with a label, e.g Button
    void center_next_label(const char *label, float alignment) {
        ImGuiStyle &style = ImGui::GetStyle();

        float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
        float avail = ImGui::GetContentRegionAvail().x;

        float off = (avail - size) * alignment;
        if (off > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
    }

// center a wrapped text widget
    void text_wrapped_centered(const std::string &text) {
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

    void AlignForWidth(float width, float alignment) {
        ImGuiStyle& style = ImGui::GetStyle();
        float avail = ImGui::GetContentRegionAvail().x;
        float off = (avail - width) * alignment;
        if (off > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
    }

}

namespace ConnectionPanel {

// show to label for an input field
    void show_input_label(const char *label) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", label);
        ImGui::TableNextColumn();
    }


    bool Data::validate() const {
        if (name.size() > 64 || name.empty()) return false;
        if (host.empty()) return false;
        return true;
    }

// creates a selectable for the given team
    void team_selectable(const char *label, TeamSelection *selection, TeamSelection team, float height) {
        ImGui::TableNextColumn();
        if (ImGui::RoundedSelectable(label, team == *selection, 0, {0, height})) {
            *selection = team;
        }
    }

/// displays the connection panel, data is read and written into the input argument
    void show(Data *input) {

        auto height = ImGui::GetFontSize();

        auto style = ImGui::ScopedStyle{};
        style.push_style(ImGuiStyleVar_WindowRounding, 20);
        style.push_style(ImGuiStyleVar_WindowMinSize, {750, height * 18});
        style.push_style(ImGuiStyleVar_CellPadding, {height / 2, height / 2});
        style.push_style(ImGuiStyleVar_WindowPadding, {height, height});
        style.push_style(ImGuiStyleVar_SelectableTextAlign, {0.5, 0.5});

        ImGuiUtils::center_next_window_once();
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
        team_selectable("TEAM A", &input->team, TeamSelection::TEAM_A, height * 2);
        team_selectable("TEAM B", &input->team, TeamSelection::TEAM_B, height * 2);
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


        ImGuiUtils::center_next_label(input->status.c_str());
        ImGui::TextColored(ImGui::LIGHT_GREY, "%s", input->status.c_str());

        ImGui::End();
    }

}

std::pair<std::string, ImVec4> msg_type_to_string_and_color(MessageType typ) {
    switch (typ) {
        case MessageType::Error:
            return {"error", ImGui::RED};
        case MessageType::Warn:
            return {"warning", ImGui::ORANGE};
        case MessageType::Info:
            return {"info", ImGui::WHITE};
    }
    return {"unknown", ImGui::BLACK};
}


void Message::on_imgui() {
    auto [title, title_color] = msg_type_to_string_and_color(type);

    // add imgui unique id to the title (view docs for more info)
    title += "###" + std::to_string(id);

    auto width = 500.f;
    auto style = ImGui::ScopedStyle{};
    style.push_style(ImGuiStyleVar_WindowMinSize, {width, 0});
    style.push_color(ImGuiCol_TitleBg, title_color);
    style.push_color(ImGuiCol_TitleBgActive, title_color);
    //style.push_color(ImGuiCol_Button, ImGui::DARK_GREY);
    style.push_style(ImGuiStyleVar_WindowRounding, 20);
    style.push_style(ImGuiStyleVar_WindowPadding, {20, 20});
    style.push_style(ImGuiStyleVar_WindowTitleAlign, {0.5f, 0.5f});
    style.push_style(ImGuiStyleVar_WindowBorderSize, 2);
    style.push_color(ImGuiCol_Border, title_color);

    ImGuiUtils::center_next_window_once();
    ImGui::Begin(title.c_str(), nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse);

    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

    ImGuiUtils::text_wrapped_centered(message);

    ImGui::NewLine();

    if (ImGui::ButtonEx("close", {ImGui::GetContentRegionAvail().x, 0})) {
        should_close = true;
    }
    ImGui::End();
}

void Message::show_windows(std::vector<Message> *messages) {
    for (auto &msg: *messages) {
        msg.on_imgui();
    }

    // remove closed messages
    auto end = messages->end();
    auto begin = messages->begin();
    messages->erase(std::remove_if(begin, end, [](const Message &msg) { return msg.should_close; }), end);
}