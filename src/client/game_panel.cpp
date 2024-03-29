#include "game_panel.h"

#include <glm/gtx/transform.hpp>
#include <map>
#include <deque>
#include <chrono>

#include "Renderer/application.h"
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

long long time_now() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

/*
 * map EventType to colors
 */
ImVec4 event_to_color(const EventType &e)
{
    const auto HIGH = ImGui::RED;
    switch (e)
    {
        case EventType::PLAY_COMBI:
            return ImVec4{0.4, 0.3, 0.3, 1.0};
        case EventType::PASS:
            return ImVec4{0.3, 0.3, 0.3, 1.0};
        case EventType::GAME_START:
        case EventType::ROUND_END:
            return ImGui::BLUE;

        case EventType::WISH:
            return ImGui::WHITE;
        case EventType::BOMB:
            return ImGui::RED;

        case EventType::GRAND_TICHU:
        case EventType::SMALL_TICHU:
            return ImGui::YELLOW;
        case EventType::PLAYER_FINISHED:
        case EventType::STICH_END:
        case EventType::SWAP_IN:
        case EventType::SWAP_OUT:
            return ImGui::GREEN;

        default:
            return ImGui::WHITE;
    }
}

/// shows the framebuffer in the viewport window
void show_main_framebuffer()
{
    ImGui::Begin("viewport");
    auto size = Application::get_viewport_size();
    ImGui::Image(Renderer::get_frame_buffer().get_attachment(0), {(float)size.x, (float)size.y});
    ImGui::End();
}

// fix the next window to a relative position to the viewport
void rel_fix_next_window(float x, float y, glm::vec2 pivot = {0.5, 0.5})
{
    glm::vec2 size = Application::get_viewport_size();
    glm::vec2 pos = Application::get_viewport_pos();
    ImGui::SetNextWindowPos({size.x * x + pos.x, size.y * y + pos.y}, ImGuiCond_Always, {pivot.x, pivot.y});
}

/*
 * ImGui frameless window, useful for displaying text on the viewport
 */
void begin_frameless_window(const std::string &id)
{
    ImGui::Begin(id.c_str(), nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize |
                 ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav);
}

void hovering_text(const std::string &id, const std::string &text, float x, float y)
{
    auto style = ImGui::ScopedStyle();
    rel_fix_next_window(x, y);
    begin_frameless_window(id);
    ImGui::Text("%s", text.c_str());
    ImGui::End();
}

namespace GamePanel
{
    // some constants for general styling
    float window_left = .5f, window_right = .5f, window_bottom = 0, window_top = 1;
    glm::vec2 window_size{1, 1};
    const float side_padding = 0.2f;
    const glm::vec2 _card_size = {0.1f, 0.15f};

    const float spread_anim_len = 0.5f;
    const float hover_anim_len = 0.1f;
    const float collect_anim_len = 1.f;

    static Texture card_frame{};
    const std::map<Card, const char *> card_to_asset = {
            {Card(TWO, SCHWARZ), "row-1-column-1"},
            {Card(THREE, SCHWARZ), "row-1-column-2"},
            {Card(FOUR, SCHWARZ), "row-1-column-3"},
            {Card(FIVE, SCHWARZ), "row-1-column-4"},
            {Card(SIX, SCHWARZ), "row-1-column-5"},
            {Card(SEVEN, SCHWARZ), "row-1-column-6"},
            {Card(EIGHT, SCHWARZ), "row-1-column-7"},
            {Card(NINE, SCHWARZ), "row-1-column-8"},
            {Card(TEN, SCHWARZ), "row-1-column-9"},
            {Card(JACK, SCHWARZ), "row-1-column-10"},
            {Card(QUEEN, SCHWARZ), "row-1-column-11"},
            {Card(KING, SCHWARZ), "row-1-column-12"},
            {Card(ACE, SCHWARZ), "row-1-column-13"},

            {Card(TWO, RED), "row-2-column-1"},
            {Card(THREE, RED), "row-2-column-2"},
            {Card(FOUR, RED), "row-2-column-3"},
            {Card(FIVE, RED), "row-2-column-4"},
            {Card(SIX, RED), "row-2-column-5"},
            {Card(SEVEN, RED), "row-2-column-6"},
            {Card(EIGHT, RED), "row-2-column-7"},
            {Card(NINE, RED), "row-2-column-8"},
            {Card(TEN, RED), "row-2-column-9"},
            {Card(JACK, RED), "row-2-column-10"},
            {Card(QUEEN, RED), "row-2-column-11"},
            {Card(KING, RED), "row-2-column-12"},
            {Card(ACE, RED), "row-2-column-13"},

            {Card(TWO, BLUE), "row-3-column-1"},
            {Card(THREE, BLUE), "row-3-column-2"},
            {Card(FOUR, BLUE), "row-3-column-3"},
            {Card(FIVE, BLUE), "row-3-column-4"},
            {Card(SIX, BLUE), "row-3-column-5"},
            {Card(SEVEN, BLUE), "row-3-column-6"},
            {Card(EIGHT, BLUE), "row-3-column-7"},
            {Card(NINE, BLUE), "row-3-column-8"},
            {Card(TEN, BLUE), "row-3-column-9"},
            {Card(JACK, BLUE), "row-3-column-10"},
            {Card(QUEEN, BLUE), "row-4-column-11"},
            {Card(KING, BLUE), "row-3-column-12"},
            {Card(ACE, BLUE), "row-3-column-13"},

            {Card(TWO, GREEN), "row-4-column-1"},
            {Card(THREE, GREEN), "row-4-column-2"},
            {Card(FOUR, GREEN), "row-4-column-3"},
            {Card(FIVE, GREEN), "row-4-column-4"},
            {Card(SIX, GREEN), "row-4-column-5"},
            {Card(SEVEN, GREEN), "row-4-column-6"},
            {Card(EIGHT, GREEN), "row-4-column-7"},
            {Card(NINE, GREEN), "row-4-column-8"},
            {Card(TEN, GREEN), "row-4-column-9"},
            {Card(JACK, GREEN), "row-4-column-10"},
            {Card(QUEEN, GREEN), "row-4-column-11"},
            {Card(KING, GREEN), "row-4-column-12"},
            {Card(ACE, GREEN), "row-4-column-13"},

            {Card(SPECIAL, GREEN), "row-1-column-0"},   // phoenix
            {Card(SPECIAL, RED), "row-2-column-0"},     // dragon
            {Card(SPECIAL, BLUE), "row-3-column-0"},    // dog
            {Card(SPECIAL, SCHWARZ), "row-4-column-0"}, // one
    };
    static std::map<Card, Texture> card_to_texture;
    std::vector<Card> mahjong_selection = {
            Card(TWO, SCHWARZ),
            Card(THREE, SCHWARZ),
            Card(FOUR, SCHWARZ),
            Card(FIVE, SCHWARZ),
            Card(SIX, SCHWARZ),
            Card(SEVEN, SCHWARZ),
            Card(EIGHT, SCHWARZ),
            Card(NINE, SCHWARZ),
            Card(TEN, SCHWARZ),
            Card(JACK, SCHWARZ),
            Card(QUEEN, SCHWARZ),
            Card(KING, SCHWARZ),
            Card(ACE, SCHWARZ),
    };

/* used for ease in / out animation
* t: time since action began
* duration: time it takes to go from 0 to 1
*/
    float animate(float t, float duration)
    {
        t /= duration;
        if (t <= 0.5f)
            return 2.0f * t * t;
        t -= 0.5f;
        if (t <= 0.5f)
            return 2.0f * t * (1.0f - t) + 0.5f;
        return 1;
    }

    float seconds_since(long long time)
    {
        long long diff = time_now() - time;
        float secs = (float)(diff) / 1000.f;
        return secs;
    }

    void load_textures()
    {
        card_frame = Texture::load("assets/frame.png");

        for (auto [card, asset] : card_to_asset)
        {
            std::string path = "assets/";
            path += asset + std::string(".png");
            auto texture = Texture::load(path);
            card_to_texture.insert({card, texture});
        }
    }

    void show_waiting_window(const char *text = "waiting for other players...")
    {
        ImGuiUtils::center_next_in_viewport(ImGuiCond_Always);
        auto scope = ImGui::ScopedStyle{};
        scope.push_style(ImGuiStyleVar_WindowPadding, {20, 20});
        ImGui::Begin(text, nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration);
        ImGuiUtils::center_next_label(text);
        ImGui::Text("%s", text);
        ImGui::End();
    }

    void show_lobby_wait(const Data &data)
    {
        std::string wait_text = "waiting (" + std::to_string(data.game_state.get_players().size()) + "/ 4)...";
        hovering_text("wait_text", wait_text, .5f, .5f);
    }

    Texture get_card_texture(const std::optional<Card> &card)
    {
        Texture card_texture;
        if (card.has_value() && card_to_texture.contains(*card))
        {
            card_texture = card_to_texture.at(*card);
        }
        else if (card.has_value())
        {
            WARN("unknown card found");
            card_texture = card_frame;
        }
        else
        {
            card_texture = card_frame;
        }
        return card_texture;
    }

    std::string player_display_name(const Player &player, const Data &data)
    {
        std::string name;
        if (data.game_state.get_game_phase() != GamePhase::PREGAME)
        switch (player.get_team())
        {
            case Team::A:
                name = "[A] ";
                break;
            case Team::B:
                name = "[B] ";
                break;
            default:
                break;
        }
        name += player.get_player_name();
        if (player.get_id() == data.player_id)
            name += "(you)";
        return name;
    }

    void draw_card(const glm::vec2 &pos, const glm::vec2 &size, const std::optional<Card> &card, float angle = 0, bool selected = false)
    {
        if (selected)
        {
            glm::vec2 outline_size = size * 1.05f;
            glm::vec2 offset = (outline_size - size) / 2.f;
            Renderer::rect(pos - offset, outline_size, RGBA(125, 244, 255), angle);
        }
        Texture card_texture = get_card_texture(card);
        Renderer::rect_impl(pos, size, RGBA(255), card_texture, angle);
    }

    bool is_selected(const Card &card, const Data &data)
    {
        for (const auto &c : data.selected_cards)
        {
            if (c == card)
                return true;
        }
        return false;
    }

// returns the calculated card positions, also takes hovered_card_index into account
    std::vector<float> calculate_card_positions(int n_cards, glm::vec2 size, float spread_start, float spread_end, int hover_index)
    {
        std::vector<float> positions;
        positions.resize(n_cards);

        if (n_cards == 1)
        {
            float mid = (spread_end - spread_start) / 2;
            positions.at(0) = spread_start + mid;
            return positions;
        }

        float dw = (spread_end - spread_start) / ((float)n_cards - 1);
        for (int i = 0; i < n_cards; i++)
        {
            positions.at(i) = spread_start + dw * (float)i;
        }

        if (hover_index == -1)
            return positions;

        // only recalculate positions if cards are being covered by other cards
        if (dw >= size.x)
            return positions;

        // the position of the hovered card (should be fixed)
        float hovered_pos = positions.at(hover_index);

        // spacing before hovered card
        int pre_n_cards = hover_index + 1;
        float pre_dw = (hovered_pos - spread_start) / (float)pre_n_cards;

        // spacing after hovered card
        int post_n_cards = n_cards - hover_index;
        float post_dw = (spread_end - hovered_pos) / (float)post_n_cards;

        for (int i = 0; i < n_cards; i++)
        {
            if (i < hover_index)
            {
                positions.at(i) = spread_start + pre_dw * (float)i;
            }
            else if (i > hover_index)
            {
                positions.at(i) = hovered_pos + size.x + post_dw * (float)(i - hover_index - 1);
            }
            else
            {
                positions.at(i) = hovered_pos;
            }
        }

        return positions;
    }

// return the index of the local Player
    int get_my_index(const Data &data)
    {
        if (!data.player_id.has_value())
            return -1;
        auto &players = data.game_state.get_players();
        auto it = std::find_if(players.begin(), players.end(), [&data](const player_ptr &p)
        { return data.player_id.value() == p->get_id(); });
        if (it == players.end())
        {
            return -1;
        }
        else
        {
            return (int)(it - players.begin());
        }
    }

    std::optional<player_ptr> get_me(const Data &data)
    {
        auto indx = get_my_index(data);
        if (indx == -1)
            return {};
        return data.game_state.get_players().at(indx);
    }

    bool has_me_finished(const Data &data)
    {
        auto me = get_me(data);
        if (me && (*me)->get_is_finished())
            return true;
        else
            return false;
    }

    bool is_my_turn(const Data &data)
    {
        auto c_player = data.game_state.get_current_player();
        if (!c_player.has_value())
            return false;

        if (data.game_state.get_game_phase() == GamePhase::SELECTING)
            return false;
        return c_player->get_id() == data.player_id;
    }

    void show_player_cards(Data *data)
    {
        auto card_size = _card_size * 1.5f;
        const float card_y_offset = -card_size.x * card_size.y;

        int player_index = data->my_index;
        if (player_index == -1)
        {
            WARN("Could not find local Player while trying to draw cards");
            return;
        }

        auto player = data->game_state.get_players().at(player_index);
        auto &hand = player->get_hand();
        int n_cards = hand.get_nof_cards();

        const float hover_height = .05f;

        // region where the cards are positioned
        float spread_start = window_left + side_padding * window_size.x;
        float spread_end = window_right - card_size.x - window_size.x * side_padding;
        spread_start *= animate(seconds_since(data->spread_anim_start), spread_anim_len);
        spread_end *= animate(seconds_since(data->spread_anim_start), spread_anim_len);

        auto mouse_position = Application::get_mouse_pos() - (glm::vec2)Application::get_viewport_pos();
        mouse_position /= Application::get_viewport_size();
        mouse_position *= window_size;
        glm::vec2 mouse = {mouse_position.x + window_left, window_top - mouse_position.y}; // fix opengl orientation

        int current_hover = data->hovered_card_index;
        auto x_pos = calculate_card_positions(n_cards, card_size, spread_start, spread_end, current_hover);
        std::vector<glm::vec2> positions;
        for (auto &p : x_pos)
            positions.emplace_back(p, 0);

        // change height of hovered card
        if (current_hover >= 0)
        {
            float elapsed = seconds_since(data->begin_hover_time);
            positions.at(current_hover).y = hover_height * animate(elapsed, hover_anim_len);
        }

        // check if any card is hovered
        bool hovered_any = false;
        for (int i = 0; i < n_cards; i++)
        {

            auto &card = hand.get_cards().at(i);

            float card_begin = positions.at(i).x;
            float card_end = card_begin + card_size.x;
            if (i != n_cards - 1)
                card_end = positions.at(i + 1).x;
            float card_top = positions.at(i).y + card_size.y;

            if (mouse.x > card_begin && mouse.x < card_end && mouse.y < card_top && mouse.y >= 0)
            {
                hovered_any = true;
                if (i != data->hovered_card_index)
                    data->begin_hover_time = time_now();
                data->hovered_card_index = i;

                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    if (data->selected_cards.contains(card))
                    {
                        data->selected_cards.erase(card);
                        if (card == ONE) data->selected_majong = false;
                    }
                    else
                    {
                        data->selected_cards.insert(card);
                        if (card == ONE) data->selected_majong = true;
                    }
                    auto combi = CardCombination({data->selected_cards.begin(), data->selected_cards.end()});
                    data->can_play_bomb = combi.get_combination_type() == BOMB;
                }

                break;
            }
        }

        if (!hovered_any)
        {
            data->begin_hover_time = -1;
            data->hovered_card_index = -1;
        }

        if (!hovered_any && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            // if play was pressed we should clear this set when sending the message
            if (!(data->pressed_play || data->pressed_play_bomb))
            {
                data->selected_cards.clear();
                data->can_play_bomb = false;
                data->selected_majong = false;
            }
        }

        // adjust all positions to offset
        for (auto &pos : positions)
        {
            pos.y += card_y_offset;
        }

        // draw all cards except hovered card
        for (int i = 0; i < n_cards; i++)
        {
            if (i == current_hover)
                continue;
            auto pos = positions.at(i);
            auto &card = hand.get_cards().at(i);
            draw_card(pos, card_size, card, 0, data->selected_cards.contains(card));
        }

        // draw hovered card last
        const float hover_angle = .1f;
        const float d_angle = hover_angle * 2.f / (float)n_cards;
        if (current_hover != -1)
        {
            auto &card = hand.get_cards().at(current_hover);
            auto pos = positions.at(current_hover);
            draw_card(pos, card_size, card, hover_angle - d_angle * (float)current_hover, data->selected_cards.contains(card));
        }
    }

    glm::vec2 left_won_pile_pos()
    {
        float y_pad = side_padding * 2 * window_size.y;
        return {window_left, window_bottom + y_pad * .6};
    }

    void show_left_player(int indx, const Data &data)
    {
        const auto &player = data.game_state.get_players().at(indx);
        int n_cards = player->get_nof_cards();
        int n_collected = player->get_nof_won_cards();
        float spread_anim = animate(seconds_since(data.spread_anim_start), spread_anim_len);
        float collect_anim = animate(seconds_since(data.begin_card_collect_anim), collect_anim_len);

        if (!(player->get_is_finished()))
        {
            hovering_text("left_player", player->get_player_name(), 0.2, 0.5);
        }

        float y_pad = side_padding * 2 * window_size.y;
        float mid = (window_top - window_bottom) / 2;
        float spread_start = spread_anim * (window_bottom + y_pad) + (1 - spread_anim) * mid;
        float spread_end = spread_anim * (window_top - y_pad - _card_size.x) + (1 - spread_anim) * mid;
        auto pos = calculate_card_positions(n_cards, _card_size, spread_start, spread_end, -1);

        float start_angle = -0.1f;
        float end_angle = 0.1f;
        float d_angle = (end_angle - start_angle) / (float)n_cards;
        glm::vec2 card_pos = {window_left - _card_size.x * 0.25f, 0};
        for (int i = 0; i < n_cards; i++)
        {
            card_pos.y = pos.at(i);
            // move card up because we rotate at the corner
            card_pos.y += _card_size.x;
            draw_card(card_pos, _card_size, {}, -PI / 2 + start_angle + d_angle * (float)i);
        }

        int prev_n_collected = 0;
        if (data.prev_game_state.get_players().size() == 4)
        {
            prev_n_collected = data.prev_game_state.get_players().at(indx)->get_nof_won_cards();
        }
        auto won_pos = left_won_pile_pos();
        if (collect_anim < .7f)
            n_collected = prev_n_collected;
        for (int i = 0; i < n_collected; i++)
        {
            auto angle = ((float)(hash(i) % 10) / 10.f - .5f);
            Renderer::set(Renderer::RotateMode::CENTER);
            draw_card(won_pos, _card_size, {}, PI / 2.f + angle * .2f);
            Renderer::set(Renderer::RotateMode::CORNER);
        }
    }

    glm::vec2 right_won_pile_pos()
    {
        float y_pad = side_padding * 2 * window_size.y;
        return {window_right - _card_size.x, window_top - y_pad * .6 - _card_size.y};
    }

    void show_right_player(int indx, const Data &data)
    {
        const auto &player = data.game_state.get_players().at(indx);
        int n_cards = player->get_nof_cards();
        int n_collected = player->get_nof_won_cards();

        if (!(player->get_is_finished()))
        {
            hovering_text("right_player", player->get_player_name(), 0.8, 0.5);
        }

        float spread_anim = animate(seconds_since(data.spread_anim_start), spread_anim_len);
        float collect_anim = animate(seconds_since(data.begin_card_collect_anim), collect_anim_len);

        float y_pad = side_padding * 2 * window_size.y;
        float mid = (window_top - window_bottom) / 2;
        float spread_start = spread_anim * (window_bottom + y_pad) + (1 - spread_anim) * mid;
        float spread_end = spread_anim * (window_top - y_pad - _card_size.x) + (1 - spread_anim) * mid;
        auto pos = calculate_card_positions(n_cards, _card_size, spread_start, spread_end, -1);

        float start_angle = -0.1f;
        float end_angle = 0.1f;
        float d_angle = (start_angle - end_angle) / (float)n_cards;
        glm::vec2 card_pos = {window_right + _card_size.x * 0.25f, 0};
        for (int i = 0; i < n_cards; i++)
        {
            card_pos.y = pos.at(i);
            draw_card(card_pos, _card_size, {}, PI / 2 + end_angle + d_angle * (float)i);
        }

        int prev_n_collected = 0;
        if (data.prev_game_state.get_players().size() == 4)
        {
            prev_n_collected = data.prev_game_state.get_players().at(indx)->get_nof_won_cards();
        }
        auto won_pos = right_won_pile_pos();
        if (collect_anim < .7f)
            n_collected = prev_n_collected;
        for (int i = 0; i < n_collected; i++)
        {
            auto angle = ((float)(hash(i + 200) % 10) / 10.f - .5f);
            Renderer::set(Renderer::RotateMode::CENTER);
            draw_card(won_pos, _card_size, {}, PI / 2.f + angle * .2f);
            Renderer::set(Renderer::RotateMode::CORNER);
        }
    }

    glm::vec2 top_won_pile_pos()
    {
        float x_pad = side_padding * 2 * window_size.x;
        return {window_left + x_pad * .6, window_top - _card_size.y * 0.75};
    }

    void show_top_player(int indx, const Data &data)
    {
        const auto &player = data.game_state.get_players().at(indx);
        int n_cards = player->get_nof_cards();
        int n_collected = player->get_nof_won_cards();
        float spread_anim = animate(seconds_since(data.spread_anim_start), spread_anim_len);
        float collect_anim = animate(seconds_since(data.begin_card_collect_anim), collect_anim_len);

        if (!(player->get_is_finished()))
        {
            hovering_text("top_player", player->get_player_name(), 0.5, 0.2);
        }
        float x_pad = side_padding * 2 * window_size.x;
        float spread_start = spread_anim * (window_left + x_pad);
        float spread_end = spread_anim * (window_right - x_pad - _card_size.x);
        auto top_pos = calculate_card_positions(n_cards, _card_size, spread_start, spread_end, -1);

        float start_angle = -0.1f;
        float end_angle = 0.1f;
        float d_angle = (end_angle - start_angle) / (float)n_cards;
        glm::vec2 card_pos = {0, window_top + _card_size.y * 0.25f};
        for (int i = 0; i < n_cards; i++)
        {
            card_pos.x = top_pos.at(i);
            card_pos.x += _card_size.x;
            draw_card(card_pos, _card_size, {}, PI + start_angle + d_angle * (float)i);
        }

        int prev_n_collected = 0;
        if (data.prev_game_state.get_players().size() == 4)
        {
            prev_n_collected = data.prev_game_state.get_players().at(indx)->get_nof_won_cards();
        }
        auto won_pos = top_won_pile_pos();
        if (collect_anim < .7f)
            n_collected = prev_n_collected;
        for (int i = 0; i < n_collected; i++)
        {
            auto angle = ((float)(hash(i + 100) % 10) / 10.f - .5f);
            Renderer::set(Renderer::RotateMode::CENTER);
            draw_card(won_pos, _card_size, {}, angle * .2f);
            Renderer::set(Renderer::RotateMode::CORNER);
        }
    }

    int glob_from_rel_indx(int rel_indx, int my_indx)
    {
        return (rel_indx + my_indx) % 4;
    }

    int rel_from_glob_indx(int glob_indx, int my_indx)
    {
        int indx = glob_indx - my_indx;
        if (indx >= 0)
        {
            return indx % 4;
        }
        else
        {
            return (4 + indx) % 4;
        }
    }

    void show_other_players(const Data &data)
    {
        auto &players = data.game_state.get_players();
        if (players.size() != 4)
            return;

        show_right_player(glob_from_rel_indx(1, data.my_index), data);
        show_top_player(glob_from_rel_indx(2, data.my_index), data);
        show_left_player(glob_from_rel_indx(3, data.my_index), data);
    }

    std::vector<std::pair<glm::vec2, float>> get_top_combi_transform(const CardCombination &data)
    {
        std::vector<std::pair<glm::vec2, float>> res = {};
        int n_cards = (int)data.get_cards().size();
        const float spread_start = window_left + window_size.x * side_padding * 2;
        const float spread_end = window_right - _card_size.x - window_size.x * side_padding * 2;
        auto x_pos = calculate_card_positions(n_cards, _card_size, spread_start, spread_end, -1);

        float angle = 0;
        for (int i = 0; i < n_cards; i++)
        {
            auto &pos_x = x_pos.at(i);
            float pos_y = (window_top - window_bottom) / 2.f - _card_size.y / 2.f;
            res.push_back({{pos_x, pos_y}, angle});
            angle -= .1;
        }
        return res;
    }

    void show_top_combi(const Data &data)
    {
        auto combi = data.game_state.get_active_pile().get_top_combi();
        if (!combi.has_value())
            return;

        const auto &cards = combi->get_cards();
        auto transform = get_top_combi_transform(*combi);

        int i = 0;
        for (auto [pos, rot] : transform)
        {
            draw_card(pos, _card_size, cards.at(i++), rot);
        }
    }

    void show_game_info(Data *data)
    {
        auto &players = data->game_state.get_players();
        auto c_player = data->game_state.get_current_player();

        std::string title = data->game_state.get_game_phase() == GamePhase::PREGAME ? "Lobby" : "Tichu";
        title += "###GameInfo";
        ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
        if (data->game_state.get_game_phase() != GamePhase::PREGAME)
        {
            ImGui::SeparatorText("scores");
            ImGui::BeginTable("score table", 2, ImGuiTableFlags_Borders);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Team A");
            ImGui::TableNextColumn();
            ImGui::Text("Team B");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", std::to_string(data->game_state.get_score_team_A()).c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", std::to_string(data->game_state.get_score_team_B()).c_str());

            ImGui::EndTable();
        }

        ImGui::NewLine();
        ImGui::SeparatorText("players");
        ImGui::BeginChild("Player list", {0, 0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AutoResizeY);

        for (const auto &player : players)
        {
            std::string name = player_display_name(*player, *data);

            const char *fmt = "%s";
            if (data->game_state.get_game_phase() != GamePhase::PREGAME && c_player.has_value() && *c_player == *player)
            {
                fmt = "> %s"; // indicate current Player
            }
            auto text_col = ImVec4(1, 1, 1, 1);
            if (player->get_has_skipped())
            {
                text_col = ImGui::GREY;
            }
            ImGui::TextColored(text_col, fmt, name.c_str());
        }
        ImGui::EndChild();

        auto me = get_me(*data);
        auto game_phase = data->game_state.get_game_phase();
        if (me.has_value() && (*me)->get_tichu() == Tichu::NONE && (*me)->get_nof_cards() == 14 && (game_phase == GamePhase::SWAPPING || game_phase == GamePhase::INROUND))
        {
            ImGui::Separator();
            ImGuiUtils::center_next_label("small tichu");
            if (ImGui::Button("small tichu"))
            {
                data->pressed_small_tichu = true;
            }
        }

        ImGui::NewLine();
        if (data->game_state.get_game_phase() != GamePhase::PREGAME) {
            ImGui::SeparatorText("log");

            auto avail = ImGui::GetContentRegionAvail();
            ImGui::SetNextWindowSizeConstraints({0, 0}, avail);
            ImGui::BeginChild("log", {0, 0}, ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AutoResizeY);
            for (auto &[e, msg]: data->events_log) {
                auto col = event_to_color(e.event_type);
                ImGui::PushStyleColor(ImGuiCol_Text, col);
                ImGui::TextUnformatted(msg.c_str(), msg.c_str() + msg.size());
                ImGui::PopStyleColor();
            }
            ImGui::EndChild();
        }

        ImGui::End();
    }

    void show_lobby(Data *data)
    {

        if (data->game_state.get_players().size() == 4)
        {
            rel_fix_next_window(.5f, .5f);
            begin_frameless_window("start game button");
            if (ImGui::Button("start game"))
            {
                data->pressed_start_game = true;
            }
            ImGui::End();
        }
        else
        {
            show_lobby_wait(*data);
        }
    }

    int get_winner_indx(const Data &data)
    {
        auto &players = data.game_state.get_players();
        auto &prev_players = data.prev_game_state.get_players();
        if (players.size() != prev_players.size())
            return -1;

        int winner = -1;
        for (int i = 0; i < players.size(); i++)
        {
            if (players.at(i)->get_nof_won_cards() > prev_players.at(i)->get_nof_won_cards())
            {
                winner = i;
                break;
            }
        }

        return winner;
    }

    void show_collect_anim(const Data &data)
    {
        float anim = animate(seconds_since(data.begin_card_collect_anim), .7f);
        if (anim >= 1)
            return;

        auto combi = data.prev_game_state.get_active_pile().get_top_combi();
        if (!combi.has_value())
            return;

        const auto &cards = combi->get_cards();
        int n_cards = (int)cards.size();

        auto transform = get_top_combi_transform(*combi);

        glm::vec2 won_pile_pos{};
        float won_card_angle = 0;

        int winner = get_winner_indx(data);
        if (winner == -1) return;

        winner = rel_from_glob_indx(winner, get_my_index(data));
        if (winner == 0)
        {
            won_pile_pos = {0, 0};
        }
        else if (winner == 1)
        {
            won_pile_pos = right_won_pile_pos();
            won_card_angle = -PI / 2.f;
        }
        else if (winner == 2)
        {
            won_pile_pos = top_won_pile_pos();
        }
        else if (winner == 3)
        {
            won_pile_pos = left_won_pile_pos();
            won_card_angle = PI / 2.f;
        }

        for (int i = 0; i < n_cards; i++)
        {
            auto [pos, rot] = transform.at(i);
            pos = anim * won_pile_pos + (1 - anim) * pos;
            rot = anim * won_card_angle + (1 - anim) * rot;
            Renderer::set(Renderer::RotateMode::CENTER);
            draw_card(pos, _card_size, {}, rot);
            Renderer::set(Renderer::RotateMode::CORNER);
        }
    }

    ImVec2 imgui_card_size()
    {
        auto app_size = Application::get_window_size();
        auto glm_card_size = _card_size * 10.f * std::max((float)app_size.x / 15.f, 15.f);
        return ImVec2{glm_card_size.x, glm_card_size.y};
    }

    /*
    * when we have a window which main part is a selectable card grid we want to resize the window on how many
    * cards per row we want to display
    * e.g when displaying a grid with 14 cards we want a window that is 7 cards wide and 2 cards high
    */
    void set_next_card_grid_window_size(float x, float y) {
        x += 1; // no idea why
        y += 1;
        auto card_size = imgui_card_size();
        auto padding = ImGui::GetStyle().WindowPadding + ImGui::GetStyle().CellPadding;
        ImGui::SetNextWindowSize({card_size.x * x + padding.x * 2.f + 1, card_size.y * y + padding.y * 2 + 1}, ImGuiCond_Once);
    }

    void show_selectable_card_grid(const char *label, const std::vector<Card> &cards, SelectionData *data, std::vector<bool> filter)
    {
        auto card_size = imgui_card_size();
        const ImVec2 select_pad = {5.f, 5.f};
        ImGuiUtils::item_grid(label, (int)cards.size(), card_size.x + select_pad.x * 2, [&](int i)
        {
            auto c_pos = ImGui::GetCursorPos();
            auto glob_pos = c_pos + ImGui::GetWindowPos() - ImVec2{ImGui::GetScrollX(), ImGui::GetScrollY()};

            // check if no long selectable
            if (filter.size() == cards.size() && filter.at(i)) {
                ImGui::SetCursorPos(c_pos + card_size + select_pad * 2);
                return;
            }

            // draw rectangle behind image if selected
            auto already_sel = std::find(data->selected.begin(), data->selected.end(), i);
            if (already_sel != data->selected.end()) {
                ImGui::GetWindowDrawList()->AddRectFilled(glob_pos,
                                                          glob_pos + card_size + select_pad * 2,
                                                          ImColor(0.5f, 1.f, 1.f));
            }

            ImGui::SetCursorPos(c_pos + select_pad);
            ImGui::Image(get_card_texture(cards.at(i)), card_size);

            if (ImGui::IsItemClicked()) {
                if (already_sel != data->selected.end()) {
                    data->selected.erase(already_sel);
                } else {
                    data->selected.push_back(i);
                }
            }

            ImGui::SetCursorPos(c_pos + card_size + select_pad * 2);

            while (data->n_selections < data->selected.size()) {
                data->selected.pop_front();
            } });
    }


    void show_majong_window(Data *data, bool *open) {
        ImGuiUtils::center_next_in_viewport(ImGuiCond_Once);
        set_next_card_grid_window_size(7, 2);
        auto style = ImGui::ScopedStyle{};
        style.push_style(ImGuiStyleVar_WindowTitleAlign, {.5, .5});
        ImGui::Begin("wish for a card", open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize);
        data->mahjong_wish.n_selections = 1;
        show_selectable_card_grid("mahjong", mahjong_selection, &data->mahjong_wish, {});
        ImGui::End();
    }

    void show_game(Data *data)
    {
        show_main_framebuffer();

        if (data->state_updated)
        {
            bool reset_collect_anim = false;
            auto &prev_players = data->prev_game_state.get_players();
            auto &players = data->game_state.get_players();
            if (prev_players.size() == players.size())
            {
                for (int i = 0; i < players.size(); i++)
                {
                    if (prev_players.at(i)->get_nof_won_cards() != players.at(i)->get_nof_won_cards())
                    {
                        reset_collect_anim = true;
                    }
                }
            }
            else
            {
                reset_collect_anim = true;
            }
            if (reset_collect_anim)
            {
                data->begin_card_collect_anim = -1;
            }
            data->state_updated = false;
        }

        auto prev_top_combi = data->prev_game_state.get_active_pile().get_top_combi();
        auto top_combi = data->game_state.get_active_pile().get_top_combi();
        if ((prev_top_combi && !top_combi))
        {
            if (data->begin_card_collect_anim < 0)
                data->begin_card_collect_anim = time_now();
            show_collect_anim(*data);
        }

        Renderer::clear(RGBA::from(ImGui::DARK_GREY));
        Renderer::set_camera(window_left, window_right, window_bottom, window_top);

        rel_fix_next_window(.5f, 1.f - .3f);
        begin_frameless_window("play/pass button");

        auto phase = data->game_state.get_game_phase();
        if (phase == INROUND)
        {
            if (data->can_play_bomb)
            {
                if (ImGui::Button("PLAY BOMB"))
                {
                    data->pressed_play_bomb = true;
                }
            }
            else if (is_my_turn(*data))
            {
                if (data->selected_cards.empty())
                {
                    if (ImGui::Button("PASS"))
                    {
                        data->pressed_fold = true;
                    }
                }
                else
                {
                    if (ImGui::Button("PLAY"))
                    {
                        data->pressed_play = true;
                    }
                }
            }
        }
        ImGui::End();

        if (data->show_majong_selection) {
            bool open = true;
            show_majong_window(data, &open);
            if (!open) {
                data->show_majong_selection = false;
                data->selected_majong = false;
                data->pressed_play = true;
                data->mahjong_wish.selected = {};
            } else if (data->mahjong_wish.selected.size() == 1) {
                data->show_majong_selection = false;
                data->selected_majong = false;
                data->pressed_play = true;

                data->wish = mahjong_selection.at(data->mahjong_wish.selected.at(0));
                data->mahjong_wish.selected = {};
            }
        }

        show_other_players(*data);
        show_top_combi(*data);
        show_player_cards(data);
    }

    void show_post_game(Data *data)
    {
        auto style = ImGui::ScopedStyle{};
        style.push_style(ImGuiStyleVar_WindowTitleAlign, {0.5, 0.5});
        style.push_style(ImGuiStyleVar_WindowPadding, {20, 20});
        ImGuiUtils::center_next_in_viewport(ImGuiCond_Always);
        ImGui::Begin("Game Over", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::BeginTable("score table", 2, ImGuiTableFlags_Borders);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Team A");
        ImGui::TableNextColumn();
        ImGui::Text("Team B");

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", std::to_string(data->game_state.get_score_team_A()).c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%s", std::to_string(data->game_state.get_score_team_B()).c_str());
        ImGui::EndTable();

        if (ImGui::Button("close"))
        {
            data->pressed_close = true;
        }

        ImGui::End();
    }

    void show_pre_round(Data *data)
    {
        if (data->wait_for_others_grand_tichu)
        {
            show_waiting_window();
            return;
        }

        auto card_size = imgui_card_size();
        if (data->my_index == -1)
        {
            WARN("show_pre_round: could not find player");
            return;
        }

        Renderer::clear(RGBA::from(ImGui::DARK_GREY));
        Renderer::set_camera(window_left, window_right, window_bottom, window_top);
        show_main_framebuffer();
        show_player_cards(data);

        auto style = ImGui::ScopedStyle{};
        float win_padding = 30.f;
        style.push_style(ImGuiStyleVar_WindowPadding, {win_padding, win_padding});
        style.push_style(ImGuiStyleVar_WindowTitleAlign, {.5, .5});
        ImGuiUtils::center_next_in_viewport(ImGuiCond_Once);
        ImGui::SetNextWindowSize({card_size.x * 4 + win_padding * 2 + 1, 0}, ImGuiCond_Once);
        ImGui::Begin("Grand Tichu", nullptr, ImGuiWindowFlags_NoCollapse);

        auto &cards = data->game_state.get_players().at(data->my_index)->get_hand().get_cards();
        if (cards.size() < 8)
        {
            WARN("could not find 8 cards in players hand");
        }

        // ImGuiUtils::item_grid("CardsList", 8, card_size.x, [&](int i) {
        //     ImGui::Image(get_card_texture(cards[i]), imgui_card_size());
        // });

        if (!data->wait_for_others_grand_tichu)
        {
            auto style = ImGui::ScopedStyle{};
            style.push_color(ImGuiCol_Button, ImGui::GREY);
            auto height = ImGui::GetFontSize() * 2;
            if (ImGui::Button("YES", {ImGui::GetContentRegionAvail().x, height}))
            {
                data->pressed_grand_tichu = true;
            }
            if (ImGui::Button("NO", {ImGui::GetContentRegionAvail().x, height}))
            {
                data->pressed_pass_grand_tichu = true;
            }
        }

        ImGui::End();
    }

    void show_swap_window(Data *data)
    {
        if (data->wait_for_others_swap)
        {
            show_waiting_window();
            return;
        }

        auto &players = data->game_state.get_players();

        if (data->my_index == -1)
        {
            WARN("Could not find my index");
            return;
        }

        auto style = ImGui::ScopedStyle{};
        style.push_color(ImGuiCol_Button, ImGui::GREY);
        style.push_style(ImGuiStyleVar_WindowTitleAlign, {.5, .5});
        style.push_color(ImGuiCol_TitleBg, ImGui::BLACK);
        style.push_color(ImGuiCol_TitleBgActive, ImGui::BLACK);
        ImGuiUtils::center_next_in_viewport(ImGuiCond_Once);
        set_next_card_grid_window_size(7, 2.5);
        ImGui::Begin("Select cards for swapping", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar);

        int n_total_selections = 0;
        for (int i = 0; i < 3; i++)
        {
            n_total_selections += (int)data->swap_window_data.at(i).selected.size();
        }
        bool filled_selection = n_total_selections == 3;
        float button_height = ImGui::GetFontSize() * 2.f;

        auto cards = players.at(data->my_index)->get_hand().get_cards();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
        ImGui::BeginTabBar("players");
        ImGui::PopStyleColor();
        for (int i = 2; i >= 0; i--)
        {
            ImGui::PushID(i);
            auto &p = players.at(glob_from_rel_indx(i + 1, data->my_index));
            if (p->get_is_finished())
                continue;

            // swap data of currently selected player
            auto swap = &data->swap_window_data.at(i);
            swap->n_selections = 1;
            auto name = player_display_name(*p, *data);

            // cards already selected
            auto filter = std::vector<bool>(cards.size(), false);
            for (int j = 0; j < 3; j++)
            {
                if (i == j)
                    continue;
                for (int indx : data->swap_window_data.at(j).selected)
                {
                    filter.at(indx) = true;
                }
            }

            auto reg = ImGui::GetContentRegionAvail();
            reg.y = std::abs(reg.y - (filled_selection ? button_height : 0));
            if (ImGui::BeginTabItem(name.c_str()))
            {
                ImGui::BeginChild("cards", reg);
                show_selectable_card_grid("cards_grid", cards, swap, filter);
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            ImGui::PopID();
        }

        if (filled_selection)
        {
            if (ImGui::Button("swap", ImGui::GetContentRegionAvail()))
            {
                data->pressed_swap = true;
                data->cards_for_swapping.at(0) = cards.at(data->swap_window_data.at(0).selected.at(0));
                data->cards_for_swapping.at(1) = cards.at(data->swap_window_data.at(1).selected.at(0));
                data->cards_for_swapping.at(2) = cards.at(data->swap_window_data.at(2).selected.at(0));
            }
        }

        ImGui::EndTabBar();
        ImGui::End();
    }

    void show_selecting(Data *data)
    {
        if (get_my_index(*data) != data->game_state.get_last_player_idx())
        {
            return;
        }
        auto style = ImGui::ScopedStyle{};
        style.push_color(ImGuiCol_Button, ImGui::GREY);
        ImGuiUtils::center_next_in_viewport(ImGuiCond_Once);
        style.push_style(ImGuiStyleVar_WindowTitleAlign, {.5, .5});
        ImGui::Begin("dragon trick", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize);
        auto &players = data->game_state.get_players();
        auto my_indx = get_my_index(*data);
        auto enemy1 = players.at(glob_from_rel_indx(1, my_indx));
        auto enemy2 = players.at(glob_from_rel_indx(3, my_indx));
        auto name1 = enemy1->get_player_name();
        auto name2 = enemy2->get_player_name();
        auto width = imgui_card_size().x * 4;
        if (ImGui::Button(name1.c_str(), {width, 0}))
        {
            data->pressed_select = true;
            data->selected_player = *enemy1;
        }
        if (ImGui::Button(name2.c_str(), {width, 0}))
        {
            data->pressed_select = true;
            data->selected_player = *enemy2;
        }
        ImGui::End();
    }

    void show(Data *data)
    {
        float ar = Application::get_aspect_ratio();
        window_size.x = window_size.y * ar;
        window_left = -window_size.x / 2.f;
        window_right = window_size.x / 2.f;

        data->my_index = get_my_index(*data);

        auto phase = data->game_state.get_game_phase();
        if (phase != GamePhase::SWAPPING) {
            data->wait_for_others_swap = false;
        }
        if (phase != GamePhase::PREROUND) {
            data->wait_for_others_grand_tichu = false;
        }

        show_game_info(data);

        switch (data->game_state.get_game_phase())
        {
            case GamePhase::PREGAME:
                show_lobby(data);
                break;
            case GamePhase::PREROUND:
                show_pre_round(data);
                show_game(data);
                break;
            case GamePhase::SWAPPING:
                show_swap_window(data);
                show_game(data);
                break;
            case GamePhase::SELECTING:
                show_selecting(data);
                show_game(data);
                break;
            case GamePhase::INROUND:
                show_game(data);
                break;
            case GamePhase::POSTGAME:
                show_post_game(data);
                break;
        }
    }

} // GamePane
