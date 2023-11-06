//
// Created by Manuel on 25.01.2021.
//

#ifndef TICHU_PLAYER_H
#define TICHU_PLAYER_H


#include <string>
#include <utility>
#include "hand.h"
#include "../../../../rapidjson/include/rapidjson/document.h"


class player : public serializable {
private:
    UUID _id;
    std::string _player_name;
    bool _has_folded;
    int _score;
    hand _hand;

#ifdef TICHU_SERVER
    UUID _game_id;
#endif

    /*
     * Deserialization constructor
     */
public:
    player(UUID id,
           std::string name,
           int score,
           hand hand,
           bool has_folded);

// constructors
    explicit player(std::string name);   // for client

    bool operator==(const player &other) const {
        return (_player_name == other._player_name
        && _score == other._score
        && _hand == other._hand
        && _has_folded == other._has_folded
#ifdef TICHU_SERVER
        && _game_id == other._game_id
#endif
        );
    }

    bool operator!=(const player &other) const {
        return !(*this == other);
    }

#ifdef TICHU_SERVER
    player(UUID id, std::string name);  // for server

    const UUID &get_game_id() { return _game_id; };
    void set_game_id(UUID game_id) { _game_id = std::move(game_id); };
#endif

    // accessors
    [[nodiscard]] int get_score() const noexcept;
    [[nodiscard]] const UUID &get_id() const noexcept { return _id; }
    [[nodiscard]] bool has_folded() const noexcept;
    [[nodiscard]] int get_nof_cards() const noexcept;
    [[nodiscard]] const hand& get_hand() const noexcept;
    [[nodiscard]] const std::string& get_player_name() const noexcept;

#ifdef TICHU_SERVER
    // state update functions
    bool fold(std::string& err);
    bool add_card(const card &card, std::string& err);
    std::optional<card> remove_card(const card &card_id, std::string& err);

    void wrap_up_round(std::string& err);
    void setup_round(std::string& err);
#endif


    // serialization
    static player from_json(const rapidjson::Value& json);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& alloc) const override;

};

using player_ptr = std::shared_ptr<player>;

#endif //TICHU_PLAYER_H
