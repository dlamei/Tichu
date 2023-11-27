#include "GamePanel.h"

#include <glm/gtx/transform.hpp>
#include <utility>
#include <set>

#include "GUI/Application.h"
#include "GUI/imgui_build.h"
#include "misc/cpp/imgui_stdlib.h"
#include "GUI/renderer.h"

#include "panels.h"

#define PI 3.14159265358f

/// shows the framebuffer in the viewport window
void show_main_framebuffer() {
    ImGui::Begin("viewport");
    auto size = Application::get_viewport_size();
    ImGui::Image(Renderer::get_frame_buffer().get_attachment(0), {(float) size.x, (float) size.y});
    ImGui::End();
}


// fix the next window to a relative position to the viewport
void rel_fix_next_window(float x, float y, glm::vec2 pivot = {0.5, 0.5}) {
    glm::vec2 size = Application::get_viewport_size();
    glm::vec2 pos = Application::get_viewport_pos();
    ImGui::SetNextWindowPos({size.x * x + pos.x, size.y * y + pos.y}, ImGuiCond_Always, {pivot.x, pivot.y});
}

void begin_frameless_window(const std::string &id) {
    ImGui::Begin(id.c_str(), nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize |
                 ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav);
}

void hovering_text(const std::string &id, const std::string &text, float x, float y) {
    auto style = ImGui::ScopedStyle();
    rel_fix_next_window(x, y);
    begin_frameless_window(id);
    ImGui::Text("%s", text.c_str());
    ImGui::End();
}

namespace GamePanel {
    // padding used for drawing the card spread
    const float padding = 0.2f;

    // the GamePanel namespace contains some global variables, which should be
    // fine as long as these globals only have effects in this file
    // also we only call these functions from the main thread
    static GameState state{};
    static Texture card_frame{};

    void load_textures() {
        card_frame = Texture::load("assets/frame.png");
        //player_id = std::move(_player_id);
    }

    void debug_game_state() {
        ImGui::Begin("Debug");
        auto data = json_utils::to_pretty_string(*state.to_json());
        ImGui::TextWrapped("%s", data.c_str());
        ImGui::End();
    }

    void show_waiting_text() {
        std::string wait_text = std::format("waiting ({} / 4)...", state.get_players().size());
        hovering_text("wait_text", wait_text, .5f, .5f);
    }

    void draw_card(const glm::vec2 &pos, const glm::vec2 &size, float angle = 0, bool selected = false) {

        if (selected) {
            glm::vec2 outline_size = size * 1.05f;
            glm::vec2 offset = (outline_size - size) / 2.f;
            Renderer::rect(pos - offset, outline_size, RGBA(255), angle);
        }
        Renderer::rect_impl(pos, size, RGBA(255), card_frame, angle);
    }

    bool is_selected(const Card &card, const Data &data) {
        for (const auto &c : data.selected_cards) {
            if (c == card) return true;
        }
        return false;
    }

    // returns the calculated card positions, also takes hovered_card_index into account
    std::vector<glm::vec2> calculate_card_positions(int n_cards, glm::vec2 card_size, float spread_start, float spread_end, int hover_index) {
        std::vector<glm::vec2> positions;
        positions.resize(n_cards);

        if (n_cards == 1) {
            float mid = (spread_end - spread_start) / 2;
            positions.at(0) = {spread_start + mid, 0};
            return positions;
        }

        float dw = (spread_end - spread_start) / ((float)n_cards - 1);
        for (int i = 0; i < n_cards; i++) {
            positions.at(i) = {spread_start + dw * (float) i, 0};
        }

        if (hover_index == -1) return positions;

        // only recalculate positions if cards are being covered by other cards
        if (dw >= card_size.x) return positions;

        // the position of the hovered card (should be fixed)
        float hovered_pos = positions.at(hover_index).x;

        // spacing before hovered card
        int pre_n_cards = hover_index + 1;
        float pre_dw = (hovered_pos - spread_start) / (float)pre_n_cards;

        // spacing after hovered card
        int post_n_cards = n_cards - hover_index;
        float post_dw = (spread_end - hovered_pos) / (float)post_n_cards;

        for (int i = 0; i < n_cards; i++) {
            if (i < hover_index) {
                positions.at(i).x = spread_start + pre_dw * (float)i;
            } else if (i > hover_index) {
                positions.at(i).x = hovered_pos + card_size.x + post_dw * (float)(i - hover_index - 1);
            } else {
                positions.at(i).x = hovered_pos;
            }
        }

        return positions;
    }

    // return the index of the local player
    int get_my_index(const Data &data) {
        if (!data.player_id.has_value()) return -1;
        auto &players = state.get_players();
        auto it = std::find_if(players.begin(), players.end(), [&data](const player_ptr& p) { return data.player_id.value() == p->get_id(); });
        if (it == players.end()) {
            return -1;
        } else {
            return (int)(it - players.begin());
        }
    }

    bool is_my_turn(const Data &data) {
        auto c_player = state.get_current_player();
        if (!c_player.has_value()) return false;

        return c_player->get_id() == data.player_id;
    }

    void show_player_cards(Data *data) {
        const float ar = Application::get_aspect_ratio();
        const float rel_card_width = 0.1;
        const float rel_card_height = 0.15;
        const float card_y_offset = -rel_card_width * rel_card_height;

        glm::vec2 card_size = {rel_card_width, rel_card_height * ar};

        int player_index = get_my_index(*data);
        if (player_index == -1) {
            WARN("Could not find local player while trying to draw cards");
            return;
        }

        auto &hand = state.get_players().at(player_index)->get_hand();
        int n_cards = hand.get_nof_cards();

        const float hover_height = .05f;

        // region where the cards are positioned
        const float spread_start = padding;
        const float spread_end = 1 - card_size.x - padding;

        auto mouse_position = Application::get_mouse_pos() - (glm::vec2)Application::get_viewport_pos();
        mouse_position /= Application::get_viewport_size();
        glm::vec2 mouse = {mouse_position.x, 1 - mouse_position.y}; // fix opengl orientation

        int current_hover = data->hovered_card_index;
        auto positions = calculate_card_positions(n_cards, card_size, spread_start, spread_end, current_hover);

        // change height of hovered card
        if (current_hover >= 0) {
            positions.at(current_hover).y = hover_height;
        }

        // check if any card is hovered
        bool hovered_any = false;
        for (int i = 0; i < n_cards; i++) {

            auto &card = hand.get_cards().at(i);

            float card_begin = positions.at(i).x;
            float card_end = card_begin + card_size.x;
            if (i != n_cards - 1) card_end = positions.at(i + 1).x;
            float card_top = positions.at(i).y + card_size.y;

            if (mouse.x > card_begin && mouse.x < card_end && mouse.y < card_top) {
                hovered_any = true;
                data->hovered_card_index = i;

                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    if (data->selected_cards.contains(card)) {
                        data->selected_cards.erase(card);
                    } else {
                        data->selected_cards.insert(card);
                    }
                }

                break;
            }
        }

        if (mouse.y < 0 || !hovered_any) data->hovered_card_index = -1;

        if (!hovered_any && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            // if play was pressed we should clear this set when sending the message
            if (!data->pressed_play) {
                data->selected_cards.clear();
            }
        }

        // adjust all positions to offset
        for (auto &pos : positions) {
            pos.y += card_y_offset;
        }

        // draw all cards except hovered card
        for (int i = 0; i < n_cards; i++) {
            auto pos = positions.at(i);
            auto &card = hand.get_cards().at(i);
            draw_card(pos, card_size, 0, data->selected_cards.contains(card));
        }

        // draw hovered card last
        if (current_hover != -1) {
            auto &card = hand.get_cards().at(current_hover);
            auto pos = positions.at(current_hover);
            draw_card(pos, card_size, 0, data->selected_cards.contains(card));
        }
    }

    void draw_enemy_cards(int n_left, int n_top, int n_right) {
        float ar = Application::get_aspect_ratio();
        const float rel_width = .1f;
        const float rel_height = .15f;
        glm::vec2 card_size = {rel_width, rel_height * ar};
        const float spread_start = padding * 2;
        const float spread_end = 1 - padding * 2 - card_size.x;

        auto left_positions = calculate_card_positions(n_left, card_size, spread_start, spread_end, -1);
        auto right_positions = calculate_card_positions(n_right, card_size, spread_start, spread_end, -1);
        auto top_positions = calculate_card_positions(n_top, card_size, spread_start, spread_end, -1);

        // cards on the left
        for (auto &pos : left_positions) {
            // rotate cards and adjust for aspect ratio
            std::swap(pos.x, pos.y);
            glm::vec2 size = {rel_width * ar, rel_height};
            // move up after rotation
            pos.y += card_size.x;
            pos.x = -card_size.x * 0.25f;
            draw_card(pos, size, -PI / 2);
        }

        // cards on the right
        for (auto &pos : right_positions) {

            // rotate cards and adjust for aspect ratio
            std::swap(pos.x, pos.y);
            glm::vec2 size = {rel_width * ar, rel_height};
            // move up after rotation
            pos.y += card_size.x;
            pos.x = 1 - card_size.x * 1.25f;
            draw_card(pos, size, -PI / 2);
        }

        for (auto &pos: top_positions) {
            // cards on the top
            pos.y = 1 - card_size.y * 0.75f;
            draw_card(pos, card_size);
        }
    }

    void show_enemy_cards(const Data &data) {
        auto &players = state.get_players();
        int n_players = (int)players.size();

        auto local_player_index = get_my_index(data);

        std::vector<int> n_enemy_cards{};
        n_enemy_cards.reserve(3);

        for (int i = 0; i < n_players; i++) {
            if (local_player_index == i) continue;
            n_enemy_cards.push_back(players.at(i)->get_nof_cards());
        }

        draw_enemy_cards(n_enemy_cards.at(0), n_enemy_cards.at(1), n_enemy_cards.at(2));
    }

    void show_game_stats(const Data &data) {
        auto &players = state.get_players();
        auto c_player = state.get_current_player();

        auto style = ImGui::ScopedStyle();

        std::string title = data.state == State::LOBBY ? "Lobby" : "Tichu";
        title += "###GameStats";
        ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
        switch (data.state) {
            case LOBBY:
                break;
            case GAME: {
                ImGui::SeparatorText("scores");
                ImGui::BeginTable("score table", 2, ImGuiTableFlags_Borders);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("A");
                ImGui::TableNextColumn();
                ImGui::Text("B");

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", std::to_string(state.get_score_team_A()).c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", std::to_string(state.get_score_team_B()).c_str());

                ImGui::EndTable();
                break;
            }
        }

        ImGui::NewLine();
        ImGui::SeparatorText("players");
        ImGui::BeginChild("player list", {0, 0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AutoResizeY);
        for (const auto &player : players) {
            auto &name = player->get_player_name();
            const char *fmt = "%s";
            if (data.state == State::GAME && c_player.has_value() && *c_player == *player) {
                fmt = "> %s"; // indicate current player
            }
            ImGui::Text(fmt, name.c_str());
        }
        ImGui::EndChild();
        ImGui::End();
    }

    void show_lobby(Data *data) {


        if (state.get_players().size() == 4) {
            rel_fix_next_window(.5f, .5f);
            begin_frameless_window("start game button");
            if (ImGui::Button("start game")) {
                data->pressed_start_game = true;
            }
            ImGui::End();
        } else {
            show_waiting_text();
        }
    }

    void show_game(Data *data) {
        show_main_framebuffer();

        float ar = Application::get_aspect_ratio();
        rel_fix_next_window(.5f, 1.f - .2f * ar);
        begin_frameless_window("play/fold button");

        if (is_my_turn(*data)) {
            if (data->selected_cards.empty()) {
                if (ImGui::Button("FOLD")) {
                    data->pressed_fold = true;
                }
            } else {
                if (ImGui::Button("PLAY")) {
                    data->pressed_play = true;
                }
            }
        }

        ImGui::End();

        Renderer::clear(RGBA::from(ImGui::DARK_GREY));
        Renderer::set_camera(0, 1, 0, 1);

        show_player_cards(data);
        show_enemy_cards(*data);
    }

    void show(Data *data) {
        show_game_stats(*data);

        switch (data->state) {
            case LOBBY:
                show_lobby(data);
                break;
            case GAME:
                show_game(data);
                break;
        }
    }

    void update(const GameState &_state) {
        state = _state;
    }
} // GamePanel