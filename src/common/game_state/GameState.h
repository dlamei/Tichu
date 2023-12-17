/*! \class GameState
    \brief #Central class that implements the actual Tichu game play and orchestrates the other classes.
    
 Holds the state of the whole game as well as of each round, executes and checks state modifications of
 each round (e.g. setting up rounds, turn validation, checking and clearing tricks, checking if round is finished)
 and the game (e.g. calculating and updating points, checking if game is finished).
*/

#ifndef TICHU_GAMESTATE_H
#define TICHU_GAMESTATE_H

#include <vector>
#include <string>
#include "player/Player.h"
#include "cards/DrawPile.h"
#include "cards/ActivePile.h"
#include "../Event.h"
#include "../utils.h"



enum GamePhase {
    PREGAME = 0,
    PREROUND,
    SWAPPING,
    INROUND,
    SELECTING,
    POSTGAME,
};

NLOHMANN_JSON_SERIALIZE_ENUM(GamePhase, {
    {PREGAME, "pregame"},
    {PREROUND, "preround"},
    {SWAPPING, "swapping"},
    {INROUND, "inround"},
    {SELECTING, "selecting"},
    {POSTGAME, "postgame"},
})

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
    std::optional<Card> _wish{};

    int _score_team_A{0};
    int _score_team_B{0};

    int _next_player_idx{0};
    int _starting_player_idx{0};
    int _last_player_idx{0};

    GamePhase _game_phase{GamePhase::PREGAME};
    bool _is_round_finished{false};
    bool _is_trick_finished{false};


    // from_diff constructor
    explicit GameState(UUID id);

public:

    GameState();

    // returns the index of 'Player' in the '_players' vector
    [[nodiscard]] int get_score_team_A() const { return _score_team_A; }
    [[nodiscard]] int get_score_team_B() const { return _score_team_B; }

    [[nodiscard]] int get_player_index(const Player &player) const;
    [[nodiscard]] int get_player_index(UUID player_id) const;

    [[nodiscard]] const UUID &get_id() const { return _id; }

    [[nodiscard]] GamePhase get_game_phase() const { return _game_phase; }

    [[nodiscard]] bool is_full() const { return _players.size() == _max_nof_players; }

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
        void make_teams();

        bool call_grand_tichu(const Player &player, Tichu tichu, std::string &err);
        bool call_small_tichu(const Player &player, Tichu tichu, std::string &err);

        bool dragon_selection(const Player &player, UUID selected_player, std::string &err);
        
        bool swap_cards(const Player &player, const std::vector<Card> &cards, std::vector<std::vector<Event>> &events_vec, std::string &err);
        bool check_wish(const CardCombination &combi, const Player &player, const std::optional<Card> &wish, std::string & err);

        void setup_round(std::string& err);   // server side initialization
        bool check_is_round_finished(Player &Player, std::string& err);
        void wrap_up_round(Player &Player,  std::vector<Event> &events, std::string& err);

        void setup_trick(Player &Player, std::string &err);
        bool check_is_trick_finished(Player &Player, std::string& err);
        void wrap_up_trick(Player &Player,  std::vector<Event> &events, std::string &err);

        bool add_player(const player_ptr Player, std::string& err);
        void update_current_player(Player &Player,  COMBI combi_type, std::string& err);
        bool remove_player(player_ptr Player, std::string& err);

        void setup_player(Player &Player, std::string &err);
        bool check_is_player_finished(Player &Player, std::string &err);
        void wrap_up_player(Player &Player, std::vector<Event> &events, std::string &err);


        bool play_combi(Player &Player, CardCombination& combi, std::vector<Event> &events, std::string& err, std::optional<Card> wish = {});
#endif

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GameState, _id, _players, _round_finish_order, _draw_pile, _active_pile,
                                    _wish, _score_team_A, _score_team_B, _next_player_idx, _starting_player_idx,
                                    _game_phase, _is_round_finished, _is_trick_finished, _last_player_idx)
};


#endif //TICHU_GAMESTATE_H
