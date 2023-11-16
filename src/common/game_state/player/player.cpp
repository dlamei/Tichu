#include "player.h"

#include <utility>

#include "../../exceptions/TichuException.h"

player::player(std::string name) :
        _id(UUID::create()),
        _player_name(std::move(name)),
        _team(false),
        _is_finished(false),
        _hand(hand()),
        _won_cards(won_cards_pile()),
        _tichu(false),
        _grand_tichu(false)
{ }

player::player(UUID id, std::string name, bool team, bool is_finished, hand hand, won_cards_pile won_cards, bool tichu, bool grand_tichu) :
        _id(std::move(id)),
        _player_name(std::move(name)),
        _team(team),
        _is_finished(is_finished),
        _hand(hand),
        _won_cards(won_cards),
        _tichu(tichu),
        _grand_tichu(grand_tichu)
{ }

#ifdef TICHU_SERVER
player::player(UUID id, std::string name, bool team) :
        _id(id),
        _player_name(name),
        _team(team),
        _is_finished(false),
        _hand(hand()),
        _won_cards(won_cards_pile()),
        _tichu(false),
        _grand_tichu(false)
{ }

#endif


#ifdef TICHU_SERVER

bool player::add_card_to_hand(const Card &card, std::string &err) {
    return _hand.add_card(card, err);
}

void player::remove_cards_from_hand(const card_combination &combi, std::string& err) { 
    _hand.remove_cards(combi.get_cards(), err);
}

bool player::add_cards_to_won_pile(const std::vector<card_combination> &combis, std::string& err) {
    _won_cards.add_cards(combis);
    return true;
}

void player::wrap_up_round(std::string &err) {
    _hand.wrap_up_round();
    _won_cards.wrap_up_round();
    _is_finished = false;
    _tichu = false;
    _grand_tichu = false;
}

#endif


void player::write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& alloc) const {
    string_into_json("id", _id.string(), json, alloc);
    string_into_json("player_name", _player_name, json, alloc);
    bool_into_json("team", _team, json, alloc);
    bool_into_json("is_finished", _is_finished, json, alloc);
    _hand.write_into_json_obj("hand", json, alloc);
    _won_cards.write_into_json_obj("won_cards", json, alloc);
    bool_into_json("tichu", _tichu, json, alloc);
    bool_into_json("grand_tichu", _grand_tichu, json, alloc);
}


player player::from_json(const rapidjson::Value &json) {
    //auto player_name = primitive_from_json<std::string>("player_name", json);
    //auto score = primitive_from_json<int>("score", json);
    auto id = string_from_json("id", json);
    auto player_name = string_from_json("player_name", json);
    auto team = bool_from_json("team", json);
    auto is_finished = bool_from_json("is_finished", json);
    auto hand = hand::from_json(json["hand"].GetObject());
    auto won_cards = won_cards_pile::from_json(json["won_cards"].GetObject());
    auto tichu = bool_from_json("tichu", json);
    auto grand_tichu = bool_from_json("grand_tichu", json);


    if (id && player_name && team && is_finished && hand && won_cards && tichu && grand_tichu) {
        return player { UUID(id.value()),
                        player_name.value(),
                        team.value(),
                        is_finished.value(),
                        hand.value(),
                        won_cards.value(),
                        tichu.value(),
                        grand_tichu.value()
                    };
    }
    else {
        throw TichuException("Failed to deserialize player from json. Required json entries were missing.");
    }
}
