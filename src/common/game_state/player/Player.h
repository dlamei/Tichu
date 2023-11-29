#ifndef TICHU_PLAYER_H
#define TICHU_PLAYER_H


#include <string>
#include <utility>
#include "hand.h"
#include "../cards/WonCardsPile.h"
#include <nlohmann/json.hpp>


enum class Team {
    A,
    B,
};

class Player {
private:
    UUID _id;
    std::string _player_name;
    Team _team{};
    bool _is_finished{};
    hand _hand;
    WonCardsPile _won_cards;
    bool _tichu{};
    bool _grand_tichu{};

#ifdef TICHU_SERVER
    UUID _game_id;
#endif

public:

    explicit Player(std::string name);
    Player() = default;

    bool operator==(const Player &other) const {
        return (_player_name == other._player_name
                && _hand == other._hand
                && _id == other._id
#ifdef TICHU_SERVER
                && _game_id == other._game_id
#endif
        );
    }

    bool operator!=(const Player &other) const {
        return !(*this == other);
    }

#ifdef TICHU_SERVER
    Player(UUID id, std::string name, Team team);

    const UUID &get_game_id() { return _game_id; };
    void set_game_id(UUID game_id) { _game_id = std::move(game_id); };
#endif

    // accessors
    [[nodiscard]] Team get_team() const noexcept { return _team; }

    [[nodiscard]] bool get_is_finished() const noexcept { return _is_finished; }

    [[nodiscard]] bool get_tichu() const noexcept { return _tichu; }

    [[nodiscard]] int get_grand_tichu() const noexcept { return _grand_tichu; }

    [[nodiscard]] const UUID &get_id() const noexcept { return _id; }

    [[nodiscard]] int get_nof_cards() const noexcept { return _hand.get_nof_cards(); }

    [[nodiscard]] const hand &get_hand() const noexcept { return _hand; }

    [[nodiscard]] const std::string &get_player_name() const noexcept { return this->_player_name; }

    [[nodiscard]] int get_hand_score() const noexcept { return _hand.get_score(); }

    [[nodiscard]] int get_won_score() const noexcept { return _won_cards.get_score(); }

#ifdef TICHU_SERVER
    // state update functions
    bool add_card_to_hand(const Card &card, std::string& err);
    void remove_cards_from_hand(const CardCombination &cards, std::string& err);

    void finish() { _is_finished = true; }

    bool add_cards_to_won_pile(const std::vector<CardCombination> &combis, std::string& err);
    
    void wrap_up_round(std::string& err);
    void setup_round(std::string& err) { }
#endif

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Player, _id, _player_name, _team, _is_finished, _hand, _won_cards, _tichu, _grand_tichu)
};

using player_ptr = std::shared_ptr<Player>;

#endif //TICHU_PLAYER_H
