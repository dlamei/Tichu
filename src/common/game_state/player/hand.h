//
// Created by Manuel on 27.01.2021.
//

#ifndef LAMA_HAND_H
#define LAMA_HAND_H

#include <vector>
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../cards/card.h"

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

// accessors
    int get_nof_cards() const;
    int get_score() const;
    const std::vector<card*> get_cards() const;
    bool try_get_card(const std::string& card_id, card*& hand_card) const;

#ifdef LAMA_SERVER
// state update functions
    void setup_round(std::string& err);
    bool add_card(card* card, std::string& err);
    bool remove_card(std::string card_id, card*& played_card, std::string& err);
#endif

    std::vector<card*>::iterator get_card_iterator();
};


#endif //LAMA_HAND_H
