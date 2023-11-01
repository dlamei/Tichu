#include "player.h"

#include <utility>

#include "../../exceptions/TichuException.h"

player::player(std::string name) :
_id(UUID::create()),
_player_name(std::move(name)), _has_folded(false), _score(0), _hand(hand()) {}

player::player(UUID id, std::string name,
               int score, hand hand, bool has_folded) :
        _id(std::move(id)),
        _player_name(std::move(name)),
        _hand(std::move(hand)),
        _score(score),
        _has_folded(has_folded)
{ }

#ifdef TICHU_SERVER
player::player(UUID id, std::string name) :
        _id(std::move(id)),
        _player_name(std::move(name)),
        _has_folded(false),
        _score(0),
        _hand()
{ }

#endif


int player::get_score() const noexcept {
    return _score;
}

const std::string& player::get_player_name() const noexcept {
    return this->_player_name;
}

const hand& player::get_hand() const noexcept {
    return this->_hand;
}

bool player::has_folded() const noexcept {
    return this->_has_folded;
}

int player::get_nof_cards() const noexcept {
    return _hand.get_nof_cards();
}


#ifdef TICHU_SERVER
void player::setup_round(std::string& err) {
    _has_folded = false;
    _hand.setup_round(err);
}

void player::wrap_up_round(std::string &err) {
    int cards_value = _hand.get_score();
    int new_score = _score;
    if (cards_value > 0) {
        new_score = _score + cards_value;
    } else {
        // The player got rid of all cards. Deduct 10 points
        new_score = std::max(0, _score - 10);
    }
    _score = new_score;
}

bool player::fold(std::string &err) {
    if (has_folded()) {
        err = "This player has already folded.";
        return false;
    }
    _has_folded = true;
    return true;
}

bool player::add_card(const card &card, std::string &err) {
    if (has_folded()) {
        err = "Player has already folded and is not allowed to draw any cards";
        return false;
    }
    return _hand.add_card(card, err);
}

std::optional<card> player::remove_card(const UUID &card_id, std::string &err) {
    //card = nullptr;
    if (has_folded()) {
        err = "Player has already folded and is not allowed to play any cards";
        return {};
    }
    return _hand.remove_card(card_id, err);
}

#endif


void player::write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& alloc) const {
    //unique_serializable::write_into_json(json, alloc);

    //rapidjson::Value id_val(_id.c_str(), alloc);
    //json.AddMember("id", id_val, alloc);

    json.AddMember("id", obj_to_json(_id, alloc), alloc);
    json.AddMember("player_name", string_to_json(_player_name, alloc), alloc);
    json.AddMember("has_folded", bool_to_json(_has_folded, alloc), alloc);
    json.AddMember("score", int_to_json(_score, alloc), alloc);
    json.AddMember("hand", obj_to_json(_hand, alloc), alloc);
}


player player::from_json(const rapidjson::Value &json) {
    auto player_name = primitive_from_json<std::string>("player_name", json);
    auto score = primitive_from_json<int>("score", json);
    auto hand = hand::from_json(json["hand"].GetObject());
    auto has_folded = primitive_from_json<bool>("has_folded", json);
    auto id = UUID::from_json(json);

    if (player_name && score && hand && has_folded && id) {
        return player { id.value(),
                        player_name.value(),
                        score.value(),
                        hand.value(),
                        has_folded.value()};
    }
    else {
        throw TichuException("Failed to deserialize player from json. Required json entries were missing.");
    }
}
