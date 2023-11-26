#include "panels.h"

#include "GUI/Application.h"
#include "GUI/imgui_build.h"
#include "misc/cpp/imgui_stdlib.h"
#include "GUI/renderer.h"

/// shows the framebuffer in the viewport window
void show_main_framebuffer() {
    ImGui::Begin("viewport");
    auto size = Application::get_viewport_size();
    ImGui::Image(Renderer::get_frame_buffer().get_attachment(0), {(float) size.x, (float) size.y});
    ImGui::End();
}

// centering function for the next window. will only center once
void center_next_window_once() {
    auto size = Application::get_window_size();
    ImGui::SetNextWindowPos({(float) size.x / 2.f, (float) size.y / 2.f}, ImGuiCond_Once, {0.5f, 0.5f});
}

// fix the next window to a relative position to the viewport
void rel_fix_next_window(float x, float y, glm::vec2 pivot = {0.5, 0.5}) {
    glm::vec2 size = Application::get_viewport_size();
    glm::vec2 pos = Application::get_viewport_pos();
    ImGui::SetNextWindowPos({size.x * x + pos.x, size.y * y + pos.y}, ImGuiCond_Always, {pivot.x, pivot.y});
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


// show to label for an input field
void show_input_label(const char *label) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("%s", label);
    ImGui::TableNextColumn();
}

namespace ConnectionPanel {

    bool ConnectionData::validate() const {
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
    void show(ConnectionData *input) {

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

}

std::pair<std::string, ImVec4> msg_type_to_string_and_color(MessageType typ) {
    switch (typ) {
        case MessageType::ERROR:
            return {"Error", ImGui::RED};
        case MessageType::WARN:
            return {"Warning", ImGui::ORANGE};
        case MessageType::INFO:
            return {"Info", ImGui::BLACK};
    }
    return {"unknown", ImGui::BLACK};
}


void Message::on_imgui() {
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

void Message::show_windows(std::vector<Message> *messages) {
    for (auto &msg: *messages) {
        msg.on_imgui();
    }

    // remove closed messages
    auto end = messages->end();
    auto begin = messages->begin();
    messages->erase(std::remove_if(begin, end, [](const Message &msg) { return msg.should_close; }), end);
}


void hovering_text(const std::string &id, const std::string &text, float x, float y) {
    auto style = ImGui::ScopedStyle();
    rel_fix_next_window(x, y);
    ImGui::Begin(id.c_str(), nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize |
                 ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav);
    ImGui::Text("%s", text.c_str());
    ImGui::End();
}

namespace GamePanel {

    static game_state s_state{};
    static Texture card_frame{};

    void init() {
        card_frame = Texture::load("assets/frame.png");
    }

    void debug_game_state() {
        ImGui::Begin("Debug");
        auto data = json_utils::to_pretty_string(*s_state.to_json());
        ImGui::TextWrapped("%s", data.c_str());
        ImGui::End();
    }

    void show() {
        show_main_framebuffer();
        //debug_game_state();
        std::string wait_text = std::format("waiting ({} / 4)...", s_state.get_players().size());
        hovering_text("wait_text", wait_text, .5f, .5f);
        std::string scores = std::format("{} : {}", s_state.get_score_team_A(), s_state.get_score_team_B());
        hovering_text("scores", scores, .5f, .1f);

        Renderer::clear(RGBA::from(ImGui::DARK_GREY));
        Renderer::set(Renderer::RectMode::CENTER);

        float width = 1;
        float ar = Application::get_aspect_ratio();
        float cw = 0.2;
        float ch = ar * cw * 1.5f;

        if (ch > 0.4f) {
            ch = .4f;
            cw = ch / ar / 1.5f;
        }

        Renderer::set_camera(0, width, 0, 1);

        // get mouse pos relative to the viewport
        auto mouse_pos = Application::get_mouse_pos() - (glm::vec2)Application::get_viewport_pos();
        mouse_pos /= Application::get_viewport_size();
        float mouseX = mouse_pos.x;
        float mouseY = 1 - mouse_pos.y; // fix orientation

        static int num_cards = 10;
        float pad = cw * 2;
        float cards_y = 0.1;
        float hover_offset = 0.1;
        ImGui::Begin("Test");
        ImGui::InputInt("n_cards", &num_cards);
        ImGui::End();

        for (int i = 0; i < num_cards; i++) {
            float cards_width = (width - pad);
            float dw = cards_width / (float)(num_cards - 1);

            float px1 = ((float)i * dw) + pad / 2;
            float py = cards_y;
            if (num_cards == 1) {
                px1 = width / 2.f;
            }

            float px2 = ((float)(i + 1) * dw) + pad / 2;
            bool hover = false;

            float card_begin = px1 - cw / 2;
            float card_end = px2 - cw / 2;
            if (i == num_cards - 1) card_end = px1 + cw / 2;

            if (mouseX >= card_begin && mouseX <= card_end) {
                hover = true;
                py += hover_offset;
            }

            if (hover && (mouseY > py + ch / 2 || mouseY < py - ch / 2)) {
                hover = false;
                py -= cards_y;
            }

            Renderer::rect({px1, py}, {cw, ch}, card_frame);
        }
    }

    void update(const game_state &state) {
        s_state = state;
    }
}