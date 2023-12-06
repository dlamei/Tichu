#include "GamePanel.h"

#include <glm/gtx/transform.hpp>
#include <ctime>
#include <map>
#include <deque>

#include "Renderer/Application.h"
#include "Renderer/imgui_build.h"
#include "misc/cpp/imgui_stdlib.h"
#include "Renderer/renderer.h"

#include "panels.h"

#define PI 3.14159265358f

uint32_t hash(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

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
    float window_left = .5f, window_right = .5f, window_bottom = 0, window_top = 1;
    glm::vec2 window_size{1, 1};
    const float padding = 0.2f;
    const glm::vec2 _card_size = {0.1f, 0.15f};

    const float spread_anim_len = 0.5f;
    const float hover_anim_len = 0.1f;
    const float collect_anim_len = 1.f;

    static Texture card_frame{};
    const std::map<Card, const char *> card_to_asset = {
            { Card(TWO,     SCHWARZ),     "row-1-column-1" },
            { Card(THREE,   SCHWARZ),     "row-1-column-2" },
            { Card(FOUR,    SCHWARZ),     "row-1-column-3" },
            { Card(FIVE,    SCHWARZ),     "row-1-column-4" },
            { Card(SIX,     SCHWARZ),     "row-1-column-5" },
            { Card(SEVEN,   SCHWARZ),     "row-1-column-6" },
            { Card(EIGHT,   SCHWARZ),     "row-1-column-7" },
            { Card(NINE,    SCHWARZ),     "row-1-column-8" },
            { Card(TEN,     SCHWARZ),     "row-1-column-9" },
            { Card(JACK,    SCHWARZ),     "row-1-column-10" },
            { Card(QUEEN,    SCHWARZ),    "row-1-column-11" },
            { Card(KING,    SCHWARZ),     "row-1-column-12" },
            { Card(ACE,     SCHWARZ),     "row-1-column-13" },

            { Card(TWO,     RED),     "row-2-column-1" },
            { Card(THREE,   RED),     "row-2-column-2" },
            { Card(FOUR,    RED),     "row-2-column-3" },
            { Card(FIVE,    RED),     "row-2-column-4" },
            { Card(SIX,     RED),     "row-2-column-5" },
            { Card(SEVEN,   RED),     "row-2-column-6" },
            { Card(EIGHT,   RED),     "row-2-column-7" },
            { Card(NINE,    RED),     "row-2-column-8" },
            { Card(TEN,     RED),     "row-2-column-9" },
            { Card(JACK,    RED),     "row-2-column-10" },
            { Card(QUEEN,   RED),    "row-2-column-11" },
            { Card(KING,    RED),     "row-2-column-12" },
            { Card(ACE,     RED),     "row-2-column-13" },

            { Card(TWO,     BLUE),     "row-3-column-1" },
            { Card(THREE,   BLUE),     "row-3-column-2" },
            { Card(FOUR,    BLUE),     "row-3-column-3" },
            { Card(FIVE,    BLUE),     "row-3-column-4" },
            { Card(SIX,     BLUE),     "row-3-column-5" },
            { Card(SEVEN,   BLUE),     "row-3-column-6" },
            { Card(EIGHT,   BLUE),     "row-3-column-7" },
            { Card(NINE,    BLUE),     "row-3-column-8" },
            { Card(TEN,     BLUE),     "row-3-column-9" },
            { Card(JACK,    BLUE),     "row-3-column-10" },
            { Card(QUEEN,   BLUE),     "row-4-column-11" },
            { Card(KING,    BLUE),     "row-3-column-12" },
            { Card(ACE,     BLUE),     "row-3-column-13" },

            { Card(TWO,     GREEN),     "row-4-column-1" },
            { Card(THREE,   GREEN),     "row-4-column-2" },
            { Card(FOUR,    GREEN),     "row-4-column-3" },
            { Card(FIVE,    GREEN),     "row-4-column-4" },
            { Card(SIX,     GREEN),     "row-4-column-5" },
            { Card(SEVEN,   GREEN),     "row-4-column-6" },
            { Card(EIGHT,   GREEN),     "row-4-column-7" },
            { Card(NINE,    GREEN),     "row-4-column-8" },
            { Card(TEN,     GREEN),     "row-4-column-9" },
            { Card(JACK,    GREEN),     "row-4-column-10" },
            { Card(QUEEN,   GREEN),     "row-4-column-11" },
            { Card(KING,    GREEN),     "row-4-column-12" },
            { Card(ACE,     GREEN),     "row-4-column-13" },

            { Card(SPECIAL, GREEN), "row-1-column-0"}, // phoenix
            { Card(SPECIAL, RED), "row-2-column-0"}, // dragon
            { Card(SPECIAL, BLUE), "row-3-column-0"}, // dog
            { Card(SPECIAL, SCHWARZ), "row-4-column-0"}, // one
    };
    static std::map<Card, Texture> card_to_texture;

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

    float seconds_since(long time) {
        return (float)(clock() - time) / CLOCKS_PER_SEC;
    }

    void load_textures() {
        card_frame = Texture::load("assets/frame.png");

        for (auto[card, asset] : card_to_asset) {
            std::string path = "assets/";
            path += asset + std::string(".png");
            auto texture = Texture::load(path);
            card_to_texture.insert({card, texture});
        }
    }

    void debug_game_state(const Data &data) {
        ImGui::Begin("Debug");
        json json_data;
        to_json(json_data, data.game_state);
        ImGui::TextWrapped("%s", json_data.dump(4).c_str());
        ImGui::End();
    }

    void show_waiting_text(const Data &data) {
        std::string wait_text = "waiting (" + std::to_string(data.game_state.get_players().size()) + "/ 4)...";
        hovering_text("wait_text", wait_text, .5f, .5f);
    }

    Texture get_card_texture(const std::optional<Card> &card) {
        Texture card_texture;
        if (card.has_value() && card_to_texture.contains(*card)) {
            card_texture = card_to_texture.at(*card);
        }
        else if (card.has_value()) {
            WARN("unknown card found");
            card_texture = card_frame;
        }
        else {
            card_texture = card_frame;
        }
        return card_texture;
    }

    void draw_card(const glm::vec2 &pos, const glm::vec2 &size, const std::optional<Card> &card, float angle = 0, bool selected = false) {
        if (selected) {
            glm::vec2 outline_size = size * 1.05f;
            glm::vec2 offset = (outline_size - size) / 2.f;
            Renderer::rect(pos - offset, outline_size, RGBA(255), angle);
        }
        Texture card_texture = get_card_texture(card);
        Renderer::rect_impl(pos, size, RGBA(255), card_texture, angle);
    }

    bool is_selected(const Card &card, const Data &data) {
        for (const auto &c : data.selected_cards) {
            if (c == card) return true;
        }
        return false;
    }

    // returns the calculated card positions, also takes hovered_card_index into account
    std::vector<float> calculate_card_positions(int n_cards, glm::vec2 size, float spread_start, float spread_end, int hover_index) {
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
        if (dw >= size.x) return positions;

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
                positions.at(i) = hovered_pos + size.x + post_dw * (float)(i - hover_index - 1);
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

    std::optional<player_ptr> get_me(const Data &data) {
        if (!data.player_id.has_value()) return {};
        auto &players = data.game_state.get_players();
        auto it = std::find_if(players.begin(), players.end(), [&data](const player_ptr& p) { return data.player_id.value() == p->get_id(); });
        if (it == players.end()) {
            return {};
        } else {
            return *it;
        }
    }

    bool is_my_turn(const Data &data) {
        auto c_player = data.game_state.get_current_player();
        if (!c_player.has_value()) return false;

        return c_player->get_id() == data.player_id;
    }

    void show_player_cards(Data *data) {
        auto card_size = _card_size * 1.5f;
        const float card_y_offset = -card_size.x * card_size.y;

        int player_index = get_my_index(*data);
        if (player_index == -1) {
            WARN("Could not find local Player while trying to draw cards");
            return;
        }

        auto &hand = data->game_state.get_players().at(player_index)->get_hand();
        int n_cards = hand.get_nof_cards();

        const float hover_height = .05f;

        // region where the cards are positioned
        float spread_start = window_left + padding * window_size.x;
        float spread_end = window_right - card_size.x - window_size.x * padding;
        spread_start *= animate(seconds_since(data->spread_anim_start), spread_anim_len);
        spread_end *= animate(seconds_since(data->spread_anim_start), spread_anim_len);

        auto mouse_position = Application::get_mouse_pos() - (glm::vec2)Application::get_viewport_pos();
        mouse_position /= Application::get_viewport_size();
        mouse_position *= window_size;
        glm::vec2 mouse = {mouse_position.x + window_left, window_top - mouse_position.y}; // fix opengl orientation

        int current_hover = data->hovered_card_index;
        auto x_pos = calculate_card_positions(n_cards, card_size, spread_start, spread_end, current_hover);
        std::vector<glm::vec2> positions;
        for (auto &p : x_pos) positions.emplace_back(p, 0);

        // change height of hovered card
        if (current_hover >= 0) {
            float elapsed = seconds_since(data->begin_hover_time);
            positions.at(current_hover).y = hover_height * animate(elapsed, hover_anim_len);
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
            draw_card(pos, card_size, card, 0, data->selected_cards.contains(card));
        }

        // draw hovered card last
        const float hover_angle = .1f;
        const float d_angle = hover_angle * 2.f/ (float)n_cards;
        if (current_hover != -1) {
            auto &card = hand.get_cards().at(current_hover);
            auto pos = positions.at(current_hover);
            draw_card(pos, card_size, card, hover_angle - d_angle * (float)current_hover, data->selected_cards.contains(card));
        }
    }

    glm::vec2 left_won_pile_pos() {
        float y_pad = padding * 2 * window_size.y;
        return {window_left, window_bottom + y_pad * .6};
    }

    void show_left_player(int indx, const Data &data) {
        const auto &player = data.game_state.get_players().at(indx);
        int n_cards = player->get_nof_cards();
        int n_collected = player->get_nof_won_cards();
        float spread_anim = animate(seconds_since(data.spread_anim_start), spread_anim_len);
        float collect_anim = animate(seconds_since(data.begin_card_collect_anim), collect_anim_len);

        float y_pad = padding * 2 * window_size.y;
        float mid = (window_top - window_bottom) / 2;
        float spread_start = spread_anim * (window_bottom + y_pad) + (1 - spread_anim) * mid;
        float spread_end = spread_anim * (window_top - y_pad - _card_size.x) + (1 - spread_anim) * mid;
        auto pos = calculate_card_positions(n_cards, _card_size, spread_start, spread_end, -1);

        float start_angle = -0.1f;
        float end_angle = 0.1f;
        float d_angle = (end_angle - start_angle) / (float)n_cards;
        glm::vec2 card_pos = {window_left - _card_size.x * 0.25f, 0};
        for (int i = 0; i < n_cards; i++) {
            card_pos.y = pos.at(i);
            // move card up because we rotate at the corner
            card_pos.y += _card_size.x;
            draw_card(card_pos, _card_size, {}, -PI / 2 + start_angle + d_angle * (float)i);
        }


        int prev_n_collected = 0;
        if (data.prev_game_state.get_players().size() == 4) {
            prev_n_collected = data.prev_game_state.get_players().at(indx)->get_nof_won_cards();
        }
        auto won_pos = left_won_pile_pos();
        if (collect_anim < .7f) n_collected = prev_n_collected;
        for (int i = 0; i < n_collected; i++) {
            auto angle = ((float)(hash(i) % 10) / 10.f - .5f);
            Renderer::set(Renderer::RotateMode::CENTER);
            draw_card(won_pos, _card_size, {}, PI / 2.f + angle * .2f);
            Renderer::set(Renderer::RotateMode::CORNER);
        }
    }

    glm::vec2 right_won_pile_pos() {
        float y_pad = padding * 2 * window_size.y;
        return {window_right - _card_size.x, window_top - y_pad * .6 - _card_size.y};
    }

    void show_right_player(int indx, const Data &data) {
        const auto &player = data.game_state.get_players().at(indx);
        int n_cards = player->get_nof_cards();
        int n_collected = player->get_nof_won_cards();

        float spread_anim = animate(seconds_since(data.spread_anim_start), spread_anim_len);
        float collect_anim = animate(seconds_since(data.begin_card_collect_anim), collect_anim_len);

        float y_pad = padding * 2 * window_size.y;
        float mid = (window_top - window_bottom) / 2;
        float spread_start = spread_anim * (window_bottom + y_pad) + (1 - spread_anim) * mid;
        float spread_end = spread_anim * (window_top - y_pad - _card_size.x) + (1 - spread_anim) * mid;
        auto pos = calculate_card_positions(n_cards, _card_size, spread_start, spread_end, -1);

        float start_angle = -0.1f;
        float end_angle = 0.1f;
        float d_angle = (start_angle - end_angle) / (float)n_cards;
        glm::vec2 card_pos = {window_right + _card_size.x * 0.25f, 0};
        for (int i = 0; i < n_cards; i++) {
            card_pos.y = pos.at(i);
            draw_card(card_pos, _card_size, {}, PI / 2 + end_angle + d_angle * (float)i);
        }

        int prev_n_collected = 0;
        if (data.prev_game_state.get_players().size() == 4) {
            prev_n_collected = data.prev_game_state.get_players().at(indx)->get_nof_won_cards();
        }
        auto won_pos = right_won_pile_pos();
        if (collect_anim < .7f) n_collected = prev_n_collected;
        for (int i = 0; i < n_collected; i++) {
            auto angle = ((float)(hash(i + 200) % 10) / 10.f - .5f);
            Renderer::set(Renderer::RotateMode::CENTER);
            draw_card(won_pos, _card_size, {}, PI / 2.f + angle * .2f);
            Renderer::set(Renderer::RotateMode::CORNER);
        }
    }

    glm::vec2 top_won_pile_pos() {
        float x_pad = padding * 2 * window_size.x;
        return {window_left + x_pad * .6, window_top - _card_size.y * 0.75};
    }

    void show_top_player(int indx, const Data &data) {
        const auto &player = data.game_state.get_players().at(indx);
        int n_cards = player->get_nof_cards();
        int n_collected = player->get_nof_won_cards();
        float spread_anim = animate(seconds_since(data.spread_anim_start), spread_anim_len);
        float collect_anim = animate(seconds_since(data.begin_card_collect_anim), collect_anim_len);

        float x_pad = padding * 2 * window_size.x;
        float spread_start = spread_anim * (window_left + x_pad);
        float spread_end = spread_anim * (window_right - x_pad - _card_size.x);
        auto top_pos = calculate_card_positions(n_cards, _card_size, spread_start, spread_end, -1);

        float start_angle = -0.1f;
        float end_angle = 0.1f;
        float d_angle = (end_angle - start_angle) / (float)n_cards;
        glm::vec2 card_pos = {0, window_top + _card_size.y * 0.25f};
        for (int i = 0; i < n_cards; i++) {
            card_pos.x = top_pos.at(i);
            card_pos.x += _card_size.x;
            draw_card(card_pos, _card_size, {}, PI + start_angle + d_angle * (float)i);

        }

        int prev_n_collected = 0;
        if (data.prev_game_state.get_players().size() == 4) {
            prev_n_collected = data.prev_game_state.get_players().at(indx)->get_nof_won_cards();
        }
        auto won_pos = top_won_pile_pos();
        if (collect_anim < .7f) n_collected = prev_n_collected;
        for (int i = 0; i < n_collected; i++) {
            auto angle = ((float)(hash(i + 100) % 10) / 10.f - .5f);
            Renderer::set(Renderer::RotateMode::CENTER);
            draw_card(won_pos, _card_size, {}, angle * .2f);
            Renderer::set(Renderer::RotateMode::CORNER);
        }
    }

    // example:
    // we know the last player_indx was 3 and someone has just won some cards. in what direction do the cards fly?
    // and we know that our index is 2
    // we call int indx = rel_from_global_indx(3, 2);
    // if indx == 0 we won the cards
    // if indx == 1 the person left to us
    // if indx == 2 the person at the top
    // if indx == 3 the person rigth to us
    int glob_from_rel_indx(int glob_indx, int my_indx) {
        return (glob_indx + my_indx) % 4;
    }

    int rel_from_glob_indx(int rel_indx, int my_indx) {
        int indx = rel_indx - my_indx;
        if (indx >= 0) {
            return indx % 4;
        } else {
            return 4 + indx;
        }
    }

    void show_other_players(const Data &data) {
        auto &players = data.game_state.get_players();
        if (players.size() != 4) return;

        auto my_indx = get_my_index(data);
        show_right_player(glob_from_rel_indx(1, my_indx), data);
        show_top_player(glob_from_rel_indx(2, my_indx), data);
        show_left_player(glob_from_rel_indx(3, my_indx), data);
    }

    std::vector<std::pair<glm::vec2, float>> get_top_combi_transform(const CardCombination& data) {
        std::vector<std::pair<glm::vec2, float>> res = {};
        int n_cards = (int)data.get_cards().size();
        const float spread_start = window_left + window_size.x * padding * 2;
        const float spread_end = window_right - _card_size.x - window_size.x * padding * 2;
        auto x_pos = calculate_card_positions(n_cards, _card_size, spread_start, spread_end, -1);

        float angle = 0;
        for (int i = 0; i < n_cards; i++) {
            auto &pos_x = x_pos.at(i);
            float pos_y = (window_top - window_bottom) / 2.f - _card_size.y / 2.f;
            res.push_back({{pos_x, pos_y}, angle});
            angle -= .1;
        }
        return res;
    }

    void show_top_combi(const Data &data) {
        auto combi = data.game_state.get_active_pile().get_top_combi();
        if (!combi.has_value()) return;

        const auto &cards = combi->get_cards();
        auto transform = get_top_combi_transform(*combi);

        int i = 0;
        for (auto[pos, rot] : transform) {
            draw_card(pos, _card_size, cards.at(i++), rot);
        }
    }

    bool has_player_folded(int indx, const Data &data) {
        //TODO
        return false;
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

        for (int i = 0; i < players.size(); i++) {
            auto &player = players.at(i);

            std::string name;
            switch (player->get_team()) {
                case Team::RANDOM: 
                    name = "[_]";
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
            auto text_col = ImVec4(1, 1, 1, 1);
            if (has_player_folded(i, data)) {
                text_col = ImGui::GREY;
            }
            ImGui::TextColored(text_col, fmt, name.c_str());
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

    void show_collect_anim(const Data &data) {
        float anim = animate(seconds_since(data.begin_card_collect_anim), .7f);
        if (anim >= 1) return;

        auto combi = data.prev_game_state.get_active_pile().get_top_combi();
        if (!combi.has_value()) return;

        const auto &cards = combi->get_cards();
        int n_cards = (int)cards.size();

        auto transform = get_top_combi_transform(*combi);

        glm::vec2 won_pile_pos{};
        float won_card_angle = 0;

        int winner = rel_from_glob_indx(data.prev_game_state.get_last_player_idx(), get_my_index(data));
        if (winner == 0) {
            won_pile_pos = {0, 0};
        } else if (winner == 1) {
            won_pile_pos = right_won_pile_pos();
            won_card_angle = - PI / 2.f;
        } else if (winner == 2) {
            won_pile_pos = top_won_pile_pos();
        } else if (winner == 3) {
            won_pile_pos = left_won_pile_pos();
            won_card_angle = PI / 2.f;
        }

        for (int i = 0; i < n_cards; i++) {
            auto [pos, rot] = transform.at(i);
            pos = anim * won_pile_pos + (1 - anim) * pos;
            rot = anim * won_card_angle + (1 - anim) * rot;
            Renderer::set(Renderer::RotateMode::CENTER);
            draw_card(pos, _card_size, {}, rot);
            Renderer::set(Renderer::RotateMode::CORNER);
        }
    }

    void show_game(Data *data) {
        show_main_framebuffer();

        if (data->state_updated) {
            bool reset_collect_anim = false;
            auto &prev_players = data->prev_game_state.get_players();
            auto &players = data->game_state.get_players();
            if (prev_players.size() == players.size()) {
                for (int i = 0; i < players.size(); i++) {
                    if (prev_players.at(i)->get_nof_won_cards() != players.at(i)->get_nof_won_cards()) {
                        reset_collect_anim = true;
                    }
                }
            } else {
                reset_collect_anim = true;
            }
            if (reset_collect_anim) {
                data->begin_card_collect_anim = -1;
            }
            data->state_updated = false;
        }

        auto prev_top_combi = data->prev_game_state.get_active_pile().get_top_combi();
        auto top_combi = data->game_state.get_active_pile().get_top_combi();
        if ((prev_top_combi && !top_combi)) {
            if (data->begin_card_collect_anim < 0) data->begin_card_collect_anim = clock();
            show_collect_anim(*data);
        }

        float ar = Application::get_aspect_ratio();
        Renderer::clear(RGBA::from(ImGui::DARK_GREY));
        Renderer::set_camera(window_left, window_right, window_bottom, window_top);

        rel_fix_next_window(.5f, 1.f - .3f * ar);
        begin_frameless_window("play/pass button");

        if (is_my_turn(*data)) {
            if (data->selected_cards.empty()) {
                if (ImGui::Button("PASS")) {
                    data->pressed_fold = true;
                }
            } else {
                if (ImGui::Button("PLAY")) {
                    data->pressed_play = true;
                }
            }
        }
        ImGui::End();

        show_other_players(*data);
        show_top_combi(*data);
        show_player_cards(data);
    }

    void show_swap_window(Data *data) {
        //auto local_player = get_me(*data);
        //if (!local_player.has_value()) {
        //    ERROR("could not find local player for swap_window");
        //    return;
        //}

        //auto me = local_player.value();

        int n_cards = 10;
        float win_padding = 15;

        auto app_size = Application::get_window_size();
        auto glm_card_size = _card_size * std::max((float)app_size.x, 300.f);
        auto card_size = ImVec2{glm_card_size.x, glm_card_size.y};


        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {win_padding, win_padding});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {win_padding, win_padding});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {card_size.x + win_padding * 2, card_size.y + win_padding * 2});
        ImGui::SetNextWindowSizeConstraints({card_size.x * 4, card_size.y * 2}, {(float)app_size.x * 0.6f, (float)app_size.y * 0.75f});
        ImGuiUtils::center_next_window_once();
        ImGui::Begin("Swap");

        ImGuiStyle& style = ImGui::GetStyle();
        float window_x_end = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

        float region_x = ImGui::GetContentRegionAvail().x;
        int cards_per_line = std::max(1, (int)(region_x / (card_size.x)));
        float cards_width = (float)cards_per_line * (card_size.x + win_padding);
        float x_offset = 0;
        if (cards_per_line > 1) {
            x_offset = (region_x - cards_width) / 2;
        }

        int n_selected = 2;
        static std::vector<int> selected = {};
        selected.resize(n_selected, -1);

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_offset);
        for (int n = 0; n < n_cards; n++)
        {
            ImGui::PushID(n);
            auto c_pos = ImGui::GetCursorPos();
            if (ImGui::Button("card", card_size)) {
                auto it = std::find(selected.begin(), selected.end(), n);
                if (it == selected.end()) {
                    for (int i = 1; i < n_selected; i++) {
                        selected.at(i) = selected.at(i - 1);
                        selected.at(i - 1) = n;
                    }
                } else {
                    *it = -1;
                }
            }
            ImGui::SetCursorPos(c_pos);

            auto it = std::find(selected.begin(), selected.end(), n);
            if (it != selected.end() && *it == n) {
                auto card_pos = ImGui::GetWindowPos() + ImGui::GetCursorPos();
                card_pos.y -= ImGui::GetScrollY();
                ImGui::GetWindowDrawList()->AddRectFilled(card_pos, card_pos + card_size, ImColor(1.f, 1.f, 1.f, 1.f), 7.f);
            }
            ImGui::Image(get_card_texture({}), card_size);

            float last_x2 = ImGui::GetItemRectMax().x;
            float next_x2 = last_x2 + style.ItemSpacing.x + card_size.x + x_offset;
            if (n + 1 < n_cards && next_x2 < window_x_end) {
                ImGui::SameLine();
            } else {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x_offset);
            }

            ImGui::PopID();
        }

        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
    }

    void show(Data *data) {
        float ar = Application::get_aspect_ratio();
        window_size.x = window_size.y * ar;
        window_left = -window_size.x / 2.f;
        window_right = window_size.x / 2.f;

        show_game_stats(*data);
        //show_swap_window(data);

        switch (data->panel_state) {
            case LOBBY:
                show_lobby(data);
                break;
            case GAME:
                show_game(data);
                break;
        }
    }

} // GamePane