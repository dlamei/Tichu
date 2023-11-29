#include "GamePanel.h"

#include <glm/gtx/transform.hpp>
#include <utility>
#include <set>
#include <ctime>

#include "Renderer/Application.h"
#include "Renderer/imgui_build.h"
#include "misc/cpp/imgui_stdlib.h"
#include "Renderer/renderer.h"

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
    // some constants for general styling
    const float padding = 0.2f;
    const glm::vec2 rel_card_size = {0.1f, 0.15f};

    static Texture card_frame{};

    // used for ease in / out animation
    // t: time since action began
    // duration: time it takes to go from 0 to 1
    float animate(float t, float duration)
    {
        t /= duration;
        if(t <= 0.5f) return 2.0f * t * t;
        t -= 0.5f;
        if (t <= 0.5f) return 2.0f * t * (1.0f - t) + 0.5f;
        return 1;
    }

    void load_textures() {
        card_frame = Texture::load("assets/frame.png");
    }

    void debug_game_state(const Data &data) {
        ImGui::Begin("Debug");
        json json_data;
        to_json(json_data, data.game_state);
        ImGui::TextWrapped("%s", json_data.dump(4).c_str());
        ImGui::End();
    }

    void show_waiting_text(const Data &data) {
        std::string wait_text = std::format("waiting ({} / 4)...", data.game_state.get_players().size());
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
    std::vector<float> calculate_card_positions(int n_cards, glm::vec2 card_size, float spread_start, float spread_end, int hover_index) {
        std::vector<float> positions;
        positions.resize(n_cards);

        if (n_cards == 1) {
            float mid = (spread_end - spread_start) / 2;
            positions.at(0) = spread_start + mid;
            return positions;
        }

        float dw = (spread_end - spread_start) / ((float)n_cards - 1);
        for (int i = 0; i < n_cards; i++) {
            positions.at(i) = spread_start + dw * (float) i;
        }

        if (hover_index == -1) return positions;

        // only recalculate positions if cards are being covered by other cards
        if (dw >= card_size.x) return positions;

        // the position of the hovered card (should be fixed)
        float hovered_pos = positions.at(hover_index);

        // spacing before hovered card
        int pre_n_cards = hover_index + 1;
        float pre_dw = (hovered_pos - spread_start) / (float)pre_n_cards;

        // spacing after hovered card
        int post_n_cards = n_cards - hover_index;
        float post_dw = (spread_end - hovered_pos) / (float)post_n_cards;

        for (int i = 0; i < n_cards; i++) {
            if (i < hover_index) {
                positions.at(i) = spread_start + pre_dw * (float)i;
            } else if (i > hover_index) {
                positions.at(i) = hovered_pos + card_size.x + post_dw * (float)(i - hover_index - 1);
            } else {
                positions.at(i) = hovered_pos;
            }
        }

        return positions;
    }

    // return the index of the local Player
    int get_my_index(const Data &data) {
        if (!data.player_id.has_value()) return -1;
        auto &players = data.game_state.get_players();
        auto it = std::find_if(players.begin(), players.end(), [&data](const player_ptr& p) { return data.player_id.value() == p->get_id(); });
        if (it == players.end()) {
            return -1;
        } else {
            return (int)(it - players.begin());
        }
    }

    bool is_my_turn(const Data &data) {
        auto c_player = data.game_state.get_current_player();
        if (!c_player.has_value()) return false;

        return c_player->get_id() == data.player_id;
    }

    void show_player_cards(Data *data) {
        const float ar = Application::get_aspect_ratio();
        const float card_y_offset = -rel_card_size.x * rel_card_size.y;

        glm::vec2 card_size = {rel_card_size.x, rel_card_size.y * ar};

        int player_index = get_my_index(*data);
        if (player_index == -1) {
            WARN("Could not find local Player while trying to draw cards");
            return;
        }

        auto &hand = data->game_state.get_players().at(player_index)->get_hand();
        int n_cards = hand.get_nof_cards();

        const float hover_height = .05f;

        // region where the cards are positioned
        const float spread_start = padding;
        const float spread_end = 1 - card_size.x - padding;

        auto mouse_position = Application::get_mouse_pos() - (glm::vec2)Application::get_viewport_pos();
        mouse_position /= Application::get_viewport_size();
        glm::vec2 mouse = {mouse_position.x, 1 - mouse_position.y}; // fix opengl orientation

        int current_hover = data->hovered_card_index;
        auto x_pos = calculate_card_positions(n_cards, card_size, spread_start, spread_end, current_hover);
        std::vector<glm::vec2> positions;
        for (auto &p : x_pos) positions.emplace_back(p, 0);

        // change height of hovered card
        if (current_hover >= 0) {
            float elapsed = (float)(clock() - data->begin_hover_time) / CLOCKS_PER_SEC;
            positions.at(current_hover).y = hover_height * animate(elapsed, .1f);
        }

        // check if any card is hovered
        bool hovered_any = false;
        for (int i = 0; i < n_cards; i++) {

            auto &card = hand.get_cards().at(i);

            float card_begin = positions.at(i).x;
            float card_end = card_begin + card_size.x;
            if (i != n_cards - 1) card_end = positions.at(i + 1).x;
            float card_top = positions.at(i).y + card_size.y;

            if (mouse.x > card_begin && mouse.x < card_end && mouse.y < card_top && mouse.y >= 0) {
                hovered_any = true;
                if (i != data->hovered_card_index) data->begin_hover_time = clock();
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

        if (!hovered_any) {
            data->begin_hover_time = -1;
            data->hovered_card_index = -1;
        }

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
            if (i == current_hover) continue;
            auto pos = positions.at(i);
            auto &card = hand.get_cards().at(i);
            draw_card(pos, card_size, 0, data->selected_cards.contains(card));
        }

        // draw hovered card last
        const float hover_angle = .1f;
        const float d_angle = hover_angle * 2.f/ (float)n_cards;
        if (current_hover != -1) {
            auto &card = hand.get_cards().at(current_hover);
            auto pos = positions.at(current_hover);
            draw_card(pos, card_size, hover_angle - d_angle * (float)current_hover, data->selected_cards.contains(card));
        }
    }

    void show_enemy_cards(const Data &data) {
        auto &players = data.game_state.get_players();
        int n_players = (int)players.size();

        auto local_player_index = get_my_index(data);

        std::vector<int> n_enemy_cards{};
        n_enemy_cards.reserve(3);

        for (int i = 0; i < n_players; i++) {
            if (local_player_index == i) continue;
            n_enemy_cards.push_back(players.at(i)->get_nof_cards());
        }

        int nLeft = n_enemy_cards.at(0);
        int nTop = n_enemy_cards.at(1);
        int nRight = n_enemy_cards.at(2);
        float ar = Application::get_aspect_ratio();
        glm::vec2 card_size = {rel_card_size.x, rel_card_size.y * ar};
        const float spread_start = padding * 2;
        const float spread_end = 1 - padding * 2 - card_size.x;

        auto left_pos = calculate_card_positions(nLeft, card_size, spread_start, spread_end, -1);
        auto right_pos = calculate_card_positions(nRight, card_size, spread_start, spread_end, -1);
        auto top_pos = calculate_card_positions(nTop, card_size, spread_start, spread_end, -1);

        float start_angle = -0.1f;
        float end_angle = 0.1f;
        float d_angle;

        // cards on the left
        d_angle = (end_angle - start_angle) / (float)left_pos.size();
        for (int i = 0; i < left_pos.size(); i++) {
            auto &pos_y = left_pos.at(i);
            const glm::vec2 size = {rel_card_size.x * ar, rel_card_size.y};
            // move card up because we rotate at the corner
            pos_y += card_size.x;
            float pos_x = -card_size.x * 0.25f;
            draw_card({pos_x, pos_y}, size, -PI / 2 + start_angle + d_angle * (float)i);
        }

        // cards on the right
        d_angle = (start_angle - end_angle) / (float)right_pos.size();
        for (int i = 0; i < right_pos.size(); i++) {
            auto &pos_y = right_pos.at(i);
            const glm::vec2 size = {rel_card_size.x * ar, rel_card_size.y};
            float pos_x = 1 + card_size.x * 0.25f;
            draw_card({pos_x, pos_y}, size, PI / 2 + end_angle + d_angle * (float)i);
        }

        // cards on the top
        d_angle = (end_angle - start_angle) / (float)top_pos.size();
        for (int i = 0; i < top_pos.size(); i++) {
            auto &pos_x = top_pos.at(i);
            float pos_y = 1 + card_size.y * 0.25f;
            pos_x += card_size.x;
            draw_card({pos_x, pos_y}, card_size, PI + start_angle + d_angle * (float)i);

        }
    }

    void show_top_combi(const Data &data) {
        auto combi = data.game_state.get_active_pile().get_top_combi();
        if (!combi.has_value()) return;

        const auto &cards = combi->get_cards();
        int n_cards = (int)cards.size();

        float ar = Application::get_aspect_ratio();
        glm::vec2 card_size = {rel_card_size.x, rel_card_size.y * ar};
        const float spread_start = padding * 2;
        const float spread_end = 1 - card_size.x - padding * 2;
        auto x_pos = calculate_card_positions(n_cards, card_size, spread_start, spread_end, -1);

        float angle = 0;
        for (auto &pos_x : x_pos) {
            float pos_y = .5f - card_size.y / 2.f;
            draw_card({pos_x, pos_y}, card_size, angle);
            angle -= .1;
        }
    }

    void show_game_stats(const Data &data) {
        auto &players = data.game_state.get_players();
        auto c_player = data.game_state.get_current_player();

        auto style = ImGui::ScopedStyle();

        std::string title = data.panel_state == State::LOBBY ? "Lobby" : "Tichu";
        title += "###GameStats";
        ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
        switch (data.panel_state) {
            case LOBBY:
                break;
            case GAME: {
                ImGui::SeparatorText("scores");
                ImGui::BeginTable("score table", 2, ImGuiTableFlags_Borders);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Team A");
                ImGui::TableNextColumn();
                ImGui::Text("Team B");

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", std::to_string(data.game_state.get_score_team_A()).c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", std::to_string(data.game_state.get_score_team_B()).c_str());

                ImGui::EndTable();
                break;
            }
        }

        ImGui::NewLine();
        ImGui::SeparatorText("players");
        ImGui::BeginChild("Player list", {0, 0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AutoResizeY);
        for (const auto &player : players) {
            std::string name;
            switch (player->get_team()) {
                case Team::A:
                    name = "[A] ";
                    break;
                case Team::B:
                    name = "[B] ";
                    break;
            }
            name += player->get_player_name();
            if (player->get_id() == data.player_id) name += " (you)";

            const char *fmt = "%s";
            if (data.panel_state == State::GAME && c_player.has_value() && *c_player == *player) {
                fmt = "> %s"; // indicate current Player
            }
            ImGui::Text(fmt, name.c_str());
        }
        ImGui::EndChild();
        ImGui::End();
    }

    void show_lobby(Data *data) {


        if (data->game_state.get_players().size() == 4) {
            rel_fix_next_window(.5f, .5f);
            begin_frameless_window("start game button");
            if (ImGui::Button("start game")) {
                data->pressed_start_game = true;
            }
            ImGui::End();
        } else {
            show_waiting_text(*data);
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

        show_enemy_cards(*data);
        show_top_combi(*data);
        show_player_cards(data);
    }

    void show(Data *data) {
        show_game_stats(*data);

        switch (data->panel_state) {
            case LOBBY:
                show_lobby(data);
                break;
            case GAME:
                show_game(data);
                break;
        }
    }

} // GamePanel