//
// Created by Manuel on 27.01.2021.
//

#ifndef LAMA_HAND_H
#define LAMA_HAND_H

#include <vector>

#include "../../../rapidjson/include/rapidjson/document.h"

#include "../cards/card.h"
#include "../../reactive_state/diffs/object_diff.h"

class hand : public reactive_object {

private:
    std::vector<card*> _cards;

    hand(base_params params);
    hand(base_params params, std::vector<card*> cards);
    card* remove_card(std::vector<card*>::iterator pos);
    card* remove_card(int idx);
    card* remove_card(card* card);

public:
    hand();
    ~hand();


// serializable interface
    static hand* from_json(const rapidjson::Value& json);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

// reactive_object interface
    virtual bool apply_diff_specialized(const diff* diff) override;
    diff* to_full_diff() const override;
    static hand* from_diff(const diff* obj_diff);

// accessors
    int get_nof_cards() const;
    int get_score() const;
    const std::vector<card*> get_cards() const;
    bool try_get_card(const std::string& card_id, card*& hand_card) const;

#ifdef LAMA_SERVER
#ifndef USE_DIFFS
// state update functions
    void setup_round(std::string& err);
    bool add_card(card* card, std::string& err);
    bool remove_card(std::string card_id, card*& played_card, std::string& err);
#else
// state update functions with diff
    void setup_round(object_diff& hand_diff, std::string& err);
    bool add_card(card* card, object_diff& hand_diff, std::string& err);
    bool remove_card(std::string card_id, card*& played_card, object_diff& hand_diff, std::string& err);
#endif
#endif

    std::vector<card*>::iterator get_card_iterator();
};


#endif //LAMA_HAND_H
