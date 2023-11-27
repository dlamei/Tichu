
#ifndef TICHU_GAMESTATE_H
#define TICHU_GAMESTATE_H

#include <vector>
#include <string>
#include "../../rapidjson/include/rapidjson/document.h"
#include "player/player.h"
#include "cards/draw_pile.h"
#include "cards/active_pile.h"
#include "../serialization/serializable.h"

class GameState : public serializable {
private:

    static const int _max_nof_players = 4;
    static const int _min_nof_players = 4;
    static const int _play_direction = 1;  // 1 or -1 depending on which direction is played in

    UUID _id;

    std::vector<player_ptr> _players{};
    std::vector<player> _round_finish_order{};

    draw_pile _draw_pile{};
    active_pile _active_pile{};

    int _score_team_A{0};
    int _score_team_B{0};

    int _next_player_idx{0};
    int _starting_player_idx{0};

    bool _is_started{false};
    bool _is_game_finished{false};
    bool _is_round_finished{false};
    bool _is_trick_finished{false};

    int _last_player_idx{0};

    // from_diff constructor
    explicit GameState(UUID id);

public:

    // deserialization constructor
    GameState(
            UUID id,
            std::vector<player_ptr> &players,
            std::vector<player> &round_finish_order,
            draw_pile draw_pile,
            active_pile active_pile,
            int score_team_A,
            int score_team_B,
            int next_player_idx,
            int starting_player_idx,
            bool is_started,
            bool is_game_finished,
            bool is_round_finished,
            bool is_trick_finished,
            int last_player_idx
    );

    GameState();

    // returns the index of 'player' in the '_players' vector
    [[nodiscard]] int get_score_team_A() const { return _score_team_A; }

    [[nodiscard]] int get_score_team_B() const { return _score_team_B; }

    [[nodiscard]] int get_player_index(const player &player) const;

    [[nodiscard]] const UUID &get_id() const { return _id; }

    [[nodiscard]] bool is_started() const { return _is_started; }

    [[nodiscard]] bool is_full() const { return _players.size() == _max_nof_players; }

    [[nodiscard]] bool is_game_finished() const { return _is_game_finished; }

    [[nodiscard]] bool is_round_finished() const { return _is_round_finished; }

    [[nodiscard]] bool is_trick_finished() const { return _is_trick_finished; }

    [[nodiscard]] bool is_player_in_game(const player &player) const;

    [[nodiscard]] bool is_allowed_to_play_now(const player &player) const;

    [[nodiscard]] const std::vector<player_ptr> &get_players() const { return _players; }

    [[nodiscard]] const std::vector<player> &get_round_finish_order() const { return _round_finish_order; }

    [[nodiscard]] const draw_pile &get_draw_pile() const { return _draw_pile; }

    [[nodiscard]] const active_pile &get_active_pile() const { return _active_pile; }

    [[nodiscard]] int get_last_player_idx() const { return _last_player_idx; }

    //TODO: shared_player
    std::optional<player> get_current_player() const;

#ifdef TICHU_SERVER
    // server-side state update functions
        bool start_game(std::string& err);
        bool check_is_game_over(std::string& err);
        void wrap_up_game(std::string& err);

        void setup_round(std::string& err);   // server side initialization
        bool check_is_round_finished(player &player, std::string& err);
        void wrap_up_round(player &player, std::string& err);

        void setup_trick(player &player, std::string &err);
        bool check_is_trick_finished(player &player, std::string& err);
        void wrap_up_trick(player &player, std::string &err);

        bool add_player(const player_ptr player, std::string& err);
        void update_current_player(player &player, bool is_pass, std::string& err);
        bool remove_player(player_ptr player, std::string& err);

        void setup_player(player &player, std::string &err);
        bool check_is_player_finished(player &player, std::string &err);
        void wrap_up_player(player &player, std::string &err);


        bool play_combi(player &player, CardCombination& combi, std::string& err);


#endif

// serializable interface
    static GameState from_json(const rapidjson::Value &json);

    void write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType &alloc) const override;

};


#endif //TICHU_GAMESTATE_H
