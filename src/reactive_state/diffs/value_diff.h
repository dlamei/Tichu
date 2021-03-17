//
// Created by Manuel on 05.02.2021.
//

#ifndef LAMA_VALUE_DIFF_H
#define LAMA_VALUE_DIFF_H

#include "diff.h"
#include "../value_type_helpers.h"


template <class T>
class value_diff : public diff {
private:
    T _value;

// copy constructor
    value_diff<T>(const value_diff<T>* other) :
            diff(diff::create_base_params(other->get_id(), other->get_name(), DiffType::valueDiff, other->_timestamp->clone())),
            _value(other->_value)
    { }

// deserialization constructor
    value_diff<T>(base_params params, T val) : diff(params), _value(val) { }

public:

    value_diff<T>(std::string id, std::string name, T val) :
            diff(diff::create_base_params(id, name, DiffType::valueDiff, new timestamp)),
            _value(val)
    { }

// accessors
    T get_value() const { return _value; }

// diff interface
    bool has_changes() const override {
        return true;
    }

    bool merge_specialized(const diff* other) override {
        const value_diff<T>* other_basic_type_diff = dynamic_cast<const value_diff<T>*>(other);
        if (other_basic_type_diff != nullptr) {
            if (other_basic_type_diff->_timestamp->is_newer(this->_timestamp)) {
                this->_value = other_basic_type_diff->_value;
                this->_timestamp = other_basic_type_diff->_timestamp;
                return true;
            }
        }
        return false;
    }

// serializable interface
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override {
        diff::write_into_json(json, allocator);
        json.AddMember("value", value_type_helpers::get_json_value(_value, allocator), allocator);

        json.AddMember("value_type", value_type_helpers::get_json_value_type(_value, allocator), allocator);
    }

    static value_diff<T>* from_json(rapidjson::Value& json) {
        base_params diff_base_params = diff::extract_base_params_from_json(json);
        if (json.HasMember("value")) {
            T val = json["value"].template Get<T>();
            return new value_diff(diff_base_params, val);
        }
        throw LamaException("Failed to deserialize value_diff. Field 'value' was not found. JSON was " + json_utils::to_string(&json));
    }

    diff* clone() const override {
        return new value_diff<T>(this);
    }
};


#endif //LAMA_VALUE_DIFF_H
