//
// Created by Manuel on 25.01.2021.
//

#include "player.h"

#include "../../common/utils/LamaException.h"

player::player(std::string name) : reactive_object("player") {
    this->_player_name = new reactive_value<std::string>("player_name", name);
    this->_has_folded = new reactive_value<bool>("has_folded", false);
    this->_score = new reactive_value<int>("score", 0);
    this->_hand = new hand();
}

player::player(reactive_object::base_params params) :
    reactive_object(params)
{
    this->_player_name = new reactive_value<std::string>("player_name", "no_name");
    this->_has_folded = new reactive_value<bool>("has_folded", false);
    this->_score = new reactive_value<int>("score", 0);
    this->_hand = new hand();
}

player::player(reactive_object::base_params params, reactive_value<std::string>* name,
               reactive_value<int>* score, hand *hand, reactive_value<bool>* has_folded) :
        reactive_object(params),
        _player_name(name),
        _hand(hand),
        _score(score),
        _has_folded(has_folded)
{ }

player::~player() {
    if (_player_name != nullptr) {
        delete _hand;
        delete _player_name;
        delete _score;
        delete _has_folded;

        _hand = nullptr;
        _player_name = nullptr;
        _score = nullptr;
        _has_folded = nullptr;
    }
}

#ifdef LAMA_SERVER
player::player(std::string id, std::string name) :
        reactive_object(id, "player")
{
    this->_player_name = new reactive_value<std::string>("player_name", name);
    this->_has_folded = new reactive_value<bool>("has_folded", false);
    this->_score = new reactive_value<int>("score", 0);
    this->_hand = new hand();
}

std::string player::get_game_id() {
    return _game_id;
}

void player::set_game_id(std::string game_id) {
    _game_id = game_id;
}
#endif


int player::get_score() const noexcept {
    return _score->get_value();
}

std::string player::get_player_name() const noexcept {
    return this->_player_name->get_value();
}

const hand* player::get_hand() const noexcept {
    return this->_hand;
}

bool player::has_folded() const noexcept {
    return this->_has_folded->get_value();
}

int player::get_nof_cards() const noexcept {
    return _hand->get_nof_cards();
}


#ifdef LAMA_SERVER
void player::setup_round(std::string& err) {
    _has_folded->set_value(false);
    _hand->setup_round(err);
}

void player::wrap_up_round(std::string &err) {
    int cards_value = _hand->get_score();
    int new_score = _score->get_value();
    if (cards_value > 0) {
        new_score = _score->get_value() + cards_value;
    } else {
        // The player got rid of all cards. Deduct 10 points
        new_score = std::max(0, _score->get_value() - 10);
    }
    _score->set_value(new_score);
}

bool player::fold(std::string &err) {
    if (has_folded()) {
        err = "This player has already folded.";
        return false;
    }
    _has_folded->set_value(true);
    return true;
}

bool player::add_card(card *card, std::string &err) {
    if (has_folded()) {
        err = "Player has already folded and is not allowed to draw any cards";
        return false;
    }
    return _hand->add_card(card, err);
}

bool player::remove_card(std::string card_id, card*& card, std::string &err) {
    card = nullptr;
    if (has_folded()) {
        err = "Player has already folded and is not allowed to play any cards";
        return false;
    }
    return _hand->remove_card(card_id, card, err);
}

#endif


void player::write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const {
    reactive_object::write_into_json(json, allocator);

    rapidjson::Value id_val(_id.c_str(), allocator);
    json.AddMember("id", id_val, allocator);

    rapidjson::Value name_val(rapidjson::kObjectType);
    _player_name->write_into_json(name_val, allocator);
    json.AddMember("player_name", name_val, allocator);

    rapidjson::Value has_folded_val(rapidjson::kObjectType);
    _has_folded->write_into_json(has_folded_val, allocator);
    json.AddMember("has_folded", has_folded_val, allocator);

    rapidjson::Value score_val(rapidjson::kObjectType);
    _score->write_into_json(score_val, allocator);
    json.AddMember("score", score_val, allocator);

    rapidjson::Value hand_val(rapidjson::kObjectType);
    _hand->write_into_json(hand_val, allocator);
    json.AddMember("hand", hand_val, allocator);
}


player *player::from_json(const rapidjson::Value &json) {
    if (json.HasMember("player_name")
        && json.HasMember("has_folded")
        && json.HasMember("hand"))
    {
        base_params params = reactive_object::extract_base_params(json);
        return new player(
                params,
                reactive_value<std::string>::from_json(json["player_name"].GetObject()),
                reactive_value<int>::from_json(json["score"].GetObject()),
                hand::from_json(json["hand"].GetObject()),
                reactive_value<bool>::from_json(json["has_folded"].GetObject()));
    } else {
        throw LamaException("Failed to deserialize player from json. Required json entries were missing.");
    }
}
