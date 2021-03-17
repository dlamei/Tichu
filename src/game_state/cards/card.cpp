//
// Created by Manuel on 25.01.2021.
//

#include "card.h"

#include "../../reactive_state/diffs/object_diff.h"
#include "../../common/utils/LamaException.h"


card::card(reactive_object::base_params params) : reactive_object(params) { }

card::card(reactive_object::base_params params, reactive_value<int> *val)
        : reactive_object(params), _value(val)
{ }

card::card(int val) :
        reactive_object("card"),
        _value(new reactive_value<int>("value", val))
{ }

card::~card() { }



int card::get_value() const noexcept {
    return _value->get_value();
}

bool card::can_be_played_on(const card *const other) const noexcept {
    // return true if this card has a higher or equal value OR if 'other' is Lama and this is 1
    return (other->get_value() <= this->get_value()) || (other->get_value() == 7 && this->get_value() == 1);
}

bool card::apply_diff_specialized(const diff* state_diff) {
    const object_diff* valid_diff = dynamic_cast<const object_diff*>(state_diff);
    if (valid_diff != nullptr && valid_diff->get_id() == this->_id) {
        if (valid_diff->get_timestamp()->is_newer(this->_timestamp) && valid_diff->has_changes()) {
            bool has_changed = false;
            diff* child_diff = nullptr;
            if (valid_diff->try_get_param_diff(_value->get_name(), child_diff)) {
                has_changed |= _value->apply_diff_specialized(child_diff);
            }
            return has_changed;
            // TODO update timestamp
        }
    }
    return false;
}

diff *card::to_full_diff() const {
    object_diff* card_diff = new object_diff(this->_id, this->_name);
    card_diff->add_param_diff("value", _value->to_full_diff());
    return card_diff;
}

card *card::from_diff(const diff* full_card_diff) {
    const object_diff* full_diff = dynamic_cast<const object_diff*>(full_card_diff);
    if (full_diff != nullptr && full_diff->get_name() == "card") {

        diff* value_diff = nullptr;
        if (!full_diff->try_get_param_diff("value", value_diff)) {
            std::cerr << "Failed to create from diff. 'value' was missing!" << std::endl;
        }
        return new card(reactive_object::extract_base_params(*full_diff), reactive_value<int>::from_diff(value_diff));
    } else {
        throw LamaException("Failed to create card from diff with name " + full_card_diff->get_name());
    }
}


card *card::from_json(const rapidjson::Value &json) {
    if (json.HasMember("value")) {
        base_params params = reactive_object::extract_base_params(json);
        return new card(params,
                        reactive_value<int>::from_json(json["value"].GetObject()));
    } else {
        throw LamaException("Could not parse json of card. Was missing 'id' or 'val'.");
    }
}


void card::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    reactive_object::write_into_json(json, allocator);

    rapidjson::Value value_val(rapidjson::kObjectType);
    _value->write_into_json(value_val, allocator);
    json.AddMember("value", value_val, allocator);
}






