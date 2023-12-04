
#ifndef TICHU_GAMESTATE_H
#define TICHU_GAMESTATE_H

#include <vector>
#include <string>
#include "player/Player.h"
#include "cards/DrawPile.h"
#include "cards/ActivePile.h"
#include "../utils.h"

class GameState {
private:

    static const int _max_nof_players = 4;
    static const int _min_nof_players = 4;
    static const int _play_direction = 1;  // 1 or -1 depending on which direction is played in

    UUID _id;

    std::vector<player_ptr> _players{};
    std::vector<Player> _round_finish_order{};

    DrawPile _draw_pile{};
    ActivePile _active_pile{};

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

    GameState();

    // returns the index of 'Player' in the '_players' vector
    [[nodiscard]] int get_score_team_A() const { return _score_team_A; }

    [[nodiscard]] int get_score_team_B() const { return _score_team_B; }

    [[nodiscard]] int get_player_index(const Player &player) const;

    [[nodiscard]] const UUID &get_id() const { return _id; }

    [[nodiscard]] bool is_started() const { return _is_started; }

    [[nodiscard]] bool is_full() const { return _players.size() == _max_nof_players; }

    [[nodiscard]] bool is_game_finished() const { return _is_game_finished; }

    [[nodiscard]] bool is_round_finished() const { return _is_round_finished; }

    [[nodiscard]] bool is_trick_finished() const { return _is_trick_finished; }

    [[nodiscard]] bool is_player_in_game(const Player &player) const;

    [[nodiscard]] bool is_allowed_to_play_now(const Player &player) const;

    [[nodiscard]] const std::vector<player_ptr> &get_players() const { return _players; }

    [[nodiscard]] const std::vector<Player> &get_round_finish_order() const { return _round_finish_order; }

    [[nodiscard]] const DrawPile &get_draw_pile() const { return _draw_pile; }

    [[nodiscard]] const ActivePile &get_active_pile() const { return _active_pile; }

    [[nodiscard]] int get_last_player_idx() const { return _last_player_idx; }
    [[nodiscard]] int get_next_player_idx() const { return _next_player_idx; }

    //TODO: shared_player
    [[nodiscard]] std::optional<Player> get_current_player() const;

#ifdef TICHU_SERVER
    // server-side state update functions
        bool start_game(std::string& err);
        bool check_is_game_over(std::string& err);
        void wrap_up_game(std::string& err);

        void setup_round(std::string& err);   // server side initialization
        bool check_is_round_finished(Player &Player, std::string& err);
        void wrap_up_round(Player &Player, std::string& err);

        void setup_trick(Player &Player, std::string &err);
        bool check_is_trick_finished(Player &Player, std::string& err);
        void wrap_up_trick(Player &Player, std::string &err);

        bool add_player(const player_ptr Player, std::string& err);
        void update_current_player(Player &Player, bool is_pass, std::string& err);
        bool remove_player(player_ptr Player, std::string& err);

        void setup_player(Player &Player, std::string &err);
        bool check_is_player_finished(Player &Player, std::string &err);
        void wrap_up_player(Player &Player, std::string &err);


        bool play_combi(Player &Player, CardCombination& combi, std::string& err);
#endif

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GameState, _id, _players, _round_finish_order, _draw_pile, _active_pile, _score_team_A, _score_team_B, _next_player_idx, _starting_player_idx, _is_started, _is_game_finished, _is_round_finished, _is_trick_finished, _last_player_idx)
};


#endif //TICHU_GAMESTATE_H
