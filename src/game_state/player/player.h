//
// Created by Manuel on 25.01.2021.
//

#ifndef LAMA_PLAYER_H
#define LAMA_PLAYER_H


#include <string>
#include "hand.h"
#include "../../common/utils/uuid_generator.h"
#include "../../../rapidjson/include/rapidjson/document.h"
#include "../../reactive_state/reactive_objects/reactive_object.h"
#include "../../reactive_state/reactive_objects/reactive_value.h"

class player : public reactive_object {
private:
    reactive_value<std::string>* _player_name;
    reactive_value<bool>* _has_folded;
    reactive_value<int>* _score;
    hand* _hand;

#ifdef LAMA_SERVER
    std::string _game_id;
#endif

    /*
     * Deserialization constructor
     */
    player(base_params params,
           reactive_value<std::string>* name,
           reactive_value<int>* score,
           hand* hand,
           reactive_value<bool>* has_folded);

    player(base_params params);

public:
// constructors
    explicit player(std::string name);   // for client
    ~player();

#ifdef LAMA_SERVER
    player(std::string id, std::string name);  // for server

    std::string get_game_id();
    void set_game_id(std::string game_id);
#endif

    // accessors
    int get_score() const noexcept;
    bool has_folded() const noexcept;
    int get_nof_cards() const noexcept;
    const hand* get_hand() const noexcept;
    std::string get_player_name() const noexcept;

#ifdef LAMA_SERVER
#ifndef USE_DIFFS
    // state update functions
    bool fold(std::string& err);
    bool add_card(card* card, std::string& err);
    bool remove_card(std::string card_id, card*& card, std::string& err);

    void wrap_up_round(std::string& err);
    void setup_round(std::string& err);
#else
    // state update functions with diffs
    bool fold(object_diff& player_diff, std::string& err);
    bool add_card(card* card, object_diff& player_diff, std::string& err);
    bool remove_card(std::string card_id, card*& card, object_diff& player_diff, std::string& err);

    void wrap_up_round(object_diff& player_diff, std::string& err);
    void setup_round(object_diff& player_diff, std::string& err);
#endif
#endif

    // reactive_object interface
    virtual bool apply_diff_specialized(const diff* diff) override;
    diff* to_full_diff() const override;
    static player* from_diff(const diff* obj_diff);


    // serialization
    static player* from_json(const rapidjson::Value& json);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

};


#endif //LAMA_PLAYER_H
