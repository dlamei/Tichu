//
// Created by Manuel on 09.02.2021.
//

#ifndef LAMA_TIMESTAMP_H
#define LAMA_TIMESTAMP_H

#include <chrono>

#include "../common/utils/LamaException.h"
#include "../../rapidjson/include/rapidjson/document.h"

#include "serializable.h"

class timestamp : serializable {
private:
    uint64_t _lower;
    uint64_t _upper;

    timestamp(timestamp* other) : _lower(other->_lower), _upper(other->_upper) {}

public:
    timestamp(uint64_t lower, uint64_t upper) : _lower(lower), _upper(upper) {}

    timestamp() {
        uint64_t current_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        _lower = current_time;
        _upper = current_time;
    }

    bool is_newer(timestamp* other) const {
        return this->_lower >= other->_upper || this->_upper >= other->_lower;
    }

    bool is_equal(timestamp* other) const {
        return this->_lower == other->_upper && this->_upper == other->_lower;
    }

    void set_values_to(timestamp* other) {
        this->_lower = other->_lower;
        this->_upper = other->_upper;
    }

    void update_values(timestamp* other) {
        if (other->_upper > this->_upper) {
            this->_upper = other->_upper;
        }
        if (other->_lower > this->_lower) { // TODO is this correct??
            this->_lower = other->_lower;
        }
    }

    timestamp* clone() {
        return new timestamp(this);
    }


    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override  {
        json.AddMember("lower", rapidjson::Value(_lower), allocator);
        json.AddMember("upper", rapidjson::Value(_upper), allocator);
    }

    static timestamp* from_json(const rapidjson::Value& json) {
        if (json.HasMember("lower") && json.HasMember("upper")) {
            return new timestamp(json["lower"].GetUint64(), json["upper"].GetUint64());
        } else {
            throw LamaException("Failed to deserialize _timestamp. 'lower' or 'upper' were missing.");
        }
    }
};

#endif //LAMA_TIMESTAMP_H
