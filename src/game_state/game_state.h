//
// Created by Manuel on 27.01.2021.
//

#ifndef LAMA_GAME_STATE_H
#define LAMA_GAME_STATE_H

#include <vector>
#include <string>

#include "../../rapidjson/include/rapidjson/document.h"

#include "player/player.h"
#include "cards/draw_pile.h"
#include "cards/discard_pile.h"
#include "../reactive_state/serializable.h"
#include "../reactive_state/reactive_objects/reactive_value.h"
#include "../reactive_state/reactive_objects/reactive_object.h"

class game_state : public reactive_object {
private:

    static const int _max_nof_players = 6;
    static const int _min_nof_players = 2;

    std::vector<player*> _players;
    draw_pile* _draw_pile;
    discard_pile* _discard_pile;
    reactive_value<bool>* _is_started;
    reactive_value<bool>* _is_finished;
    reactive_value<int>* _round_number;
    reactive_value<int>* _current_player_idx;
    reactive_value<int>* _play_direction;  // 1 or -1 depending on which direction is played in
    reactive_value<int>* _starting_player_idx;

    // from_diff constructor
    game_state(base_params params);

    // deserialization constructor
    game_state(
            base_params params,
            draw_pile* draw_pile,
            discard_pile* discard_pile,
            std::vector<player*>& players,
            reactive_value<bool>* is_started,
            reactive_value<bool>* is_finished,
            reactive_value<int>* current_player_idx,
            reactive_value<int>* play_direction,
            reactive_value<int>* round_number,
            reactive_value<int>* starting_player_idx);

    // returns the index of 'player' in the '_players' vector
    int get_player_index(player* player) const;
public:
    game_state();
    ~game_state();

// accessors
    bool is_full() const;
    bool is_started() const;
    bool is_finished() const;
    bool is_player_in_game(player* player) const;
    bool is_allowed_to_play_now(player* player) const;
    std::vector<player*>& get_players();
    int get_round_number() const;

    draw_pile* get_draw_pile() const;
    discard_pile* get_discard_pile() const;
    player* get_current_player() const;

#ifdef LAMA_SERVER
// server-side state update functions
#ifndef USE_DIFFS   // not USE_DIFFS
    // only state update
    void setup_round(std::string& err);   // server side initialization
    bool remove_player(player* player, std::string& err);
    bool add_player(player* player, std::string& err);
    bool start_game(std::string& err);
    bool draw_card(player* player, std::string& err);
    bool play_card(player* player, const std::string& card_id, std::string& err);
    bool fold(player* player, std::string& err);

    // end of round functions
    void update_current_player(std::string& err);
    void wrap_up_round(std::string& err);
#else   // USE_DIFFS
    // state update with diffs
    void setup_round(object_diff& game_state_diff, std::string& err);   // server side initialization
    bool remove_player(player* player, object_diff& game_state_diff, std::string& err);
    bool add_player(player* player, object_diff& game_state_diff, std::string& err);
    bool start_game(object_diff& game_state_diff, std::string& err);
    bool draw_card(player* player, object_diff& game_state_diff, std::string& err);
    bool play_card(player* player, const std::string& card_id, object_diff& game_state_diff, std::string& err);
    bool fold(player* player, object_diff& game_state_diff, std::string& err);

    // end of round functions
    void update_current_player(object_diff& game_state_diff, std::string& err);
    void wrap_up_round(object_diff& game_state_diff, std::string& err);
#endif
#endif

// serializable interface
    static game_state* from_json(const rapidjson::Value& json);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

};


#endif //LAMA_GAME_STATE_H
