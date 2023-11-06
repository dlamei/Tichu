//
// Created by Manuel on 27.01.2021.
//

#ifndef TICHU_GAME_STATE_H
#define TICHU_GAME_STATE_H

#include <vector>
#include <string>
#include "../../rapidjson/include/rapidjson/document.h"
#include "player/player.h"
#include "cards/draw_pile.h"
#include "cards/discard_pile.h"
#include "../serialization/serializable.h"

class game_state : public serializable {
private:

    static const int _max_nof_players = 4;
    static const int _min_nof_players = 4;

    UUID _id;
    std::vector<player> _players;
    draw_pile _draw_pile;
    discard_pile _discard_pile;
    bool _is_started;
    bool _is_finished;
    int _round_number;
    int _current_player_idx;
    int _play_direction;  // 1 or -1 depending on which direction is played in
    int _starting_player_idx;

    // from_diff constructor
    explicit game_state(UUID id);

public:

    // deserialization constructor
    game_state(
            UUID id,
            draw_pile draw_pile,
            discard_pile discard_pile,
            std::vector<player>& players,
            bool is_started,
            bool is_finished,
            int current_player_idx,
            int play_direction,
            int round_number,
            int starting_player_idx);

    game_state();

    // returns the index of 'player' in the '_players' vector
    [[nodiscard]] int get_player_index(const player& player) const;

    [[nodiscard]] const UUID &get_id() const { return _id; }
    [[nodiscard]] bool is_started() const { return _is_started; }
    [[nodiscard]] bool is_full() const { return _players.size() == _max_nof_players; }
    [[nodiscard]] bool is_finished() const { return _is_finished; }
    [[nodiscard]] bool is_player_in_game(const player& player) const;
    [[nodiscard]] bool is_allowed_to_play_now(const player& player) const;
    [[nodiscard]] const std::vector<player>& get_players() const;
    [[nodiscard]] int get_round_number() const { return _round_number; }

    [[nodiscard]] const draw_pile &get_draw_pile() const { return _draw_pile; }
    [[nodiscard]] const discard_pile &get_discard_pile() const { return _discard_pile; }
    //TODO: shared_player
    std::optional<player> get_current_player() const;

#ifdef TICHU_SERVER
// server-side state update functions
    void setup_round(std::string& err);   // server side initialization
    bool remove_player(const player &player, std::string& err);
    bool add_player(const player &player, std::string& err);
    bool start_game(std::string& err);
    bool draw_card(player &player, std::string& err);
    bool play_card(player &player, const card& card_id, std::string& err);
    bool fold(player &player, std::string& err);

    // end of round functions
    void update_current_player(std::string& err);
    void wrap_up_round(std::string& err);
#endif

// serializable interface
    static game_state from_json(const rapidjson::Value& json);
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& alloc) const override;

};


#endif //TICHU_GAME_STATE_H
