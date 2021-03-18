//
// Created by Manuel on 04.02.2021.
//

#ifndef LAMA_REACTIVE_VALUE_H
#define LAMA_REACTIVE_VALUE_H


#include <vector>
#include <iostream>
#include <functional>

#include "reactive_object.h"
#include "../value_type_helpers.h"
#include "../../../rapidjson/include/rapidjson/document.h"


/*
 * For bool, int, unsigned int, int64_t, uint64_t, float, double and string.
 * Container types are not supported
 */

template <class T>
class reactive_value : public reactive_object {

private:
    std::vector<std::function<void(const T)>*> _change_listeners;

protected:
    T _value;
    reactive_value(base_params params, T val) : reactive_object(params), _value(val) { }

public:
    explicit reactive_value(std::string var_name, T val) : reactive_object(var_name), _value(val) { }

    T get_value() const { return this->_value; }

    void set_value(T val) {
        if (this->_value != val) {
            this->_value = val;
            for (int i = 0; i < _change_listeners.size(); i++) {
                (*_change_listeners[i])(this->_value);   // inform listeners about new value
            }
        }
    }

    void register_change_listener(std::function<void(const T)>* change_callback){
        typename std::vector<std::function<void(const T)>*>::iterator position = std::find(_change_listeners.begin(), _change_listeners.end(), change_callback);
        if (position == _change_listeners.end()) {
            _change_listeners.push_back(change_callback);
        }
    }

    bool unregister_change_listener(std::function<void(const T)>* change_callback){
        typename std::vector<std::function<void(const T)>*>::iterator position = std::find(_change_listeners.begin(), _change_listeners.end(), change_callback);
        if (position != _change_listeners.end()) {
            _change_listeners.erase(position);
            return true;
        }
        return false;
    }

// serializable interface
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override {
        reactive_object::write_into_json(json, allocator);
        json.AddMember("value", value_type_helpers::get_json_value<decltype(_value)>(_value, allocator), allocator);
    }

    static reactive_value<T>* from_json(const rapidjson::Value& json) {
        if (json.HasMember("value")) {
            T val = json["value"].Get<T>();
            base_params params = reactive_object::extract_base_params(json);
            return new reactive_value<T>(params, val);
        }
        return nullptr;
    }
};



#endif //LAMA_REACTIVE_VALUE_H
