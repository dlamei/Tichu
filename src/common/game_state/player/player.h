//
// Created by Manuel on 25.01.2021.
//

#ifndef TICHU_PLAYER_H
#define TICHU_PLAYER_H


#include <string>
#include <utility>
#include "hand.h"
#include "won_cards_pile.h"
#include <rapidjson/document.h>


class player : public serializable {
private:
    UUID _id;
    std::string _player_name;
    bool _team;
    bool _is_finished;
    hand _hand;
    won_cards_pile _won_cards;
    bool _tichu;
    bool _grand_tichu;

#ifdef TICHU_SERVER
    UUID _game_id;
#endif

public:
    player(UUID id,
           std::string name,
           bool team,
           bool is_finished,
           hand hand,
           won_cards_pile won_cards,
           bool tichu,
           bool grand_tichu);

// constructors
    explicit player(std::string name);   // for client

    bool operator==(const player &other) const {
        return (_player_name == other._player_name
                && _hand == other._hand
#ifdef TICHU_SERVER
                && _game_id == other._game_id
#endif
        );
    }

    bool operator!=(const player &other) const {
        return !(*this == other);
    }

#ifdef TICHU_SERVER
    player(UUID id, std::string name, bool team);  // for server

    const UUID &get_game_id() { return _game_id; };
    void set_game_id(UUID game_id) { _game_id = std::move(game_id); };
#endif

    // accessors
    [[nodiscard]] bool get_team() const noexcept { return _team; }

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
    void remove_cards_from_hand(const card_combination &cards, std::string& err);

    void finish() { _is_finished = true; }

    bool add_cards_to_won_pile(const std::vector<card_combination> &combis, std::string& err);
    
    void wrap_up_round(std::string& err);
    void setup_round(std::string& err) { }
#endif


    // serialization
    static player from_json(const rapidjson::Value &json);

    virtual void write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType &alloc) const override;

};

using player_ptr = std::shared_ptr<player>;

#endif //TICHU_PLAYER_H
