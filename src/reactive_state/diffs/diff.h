//
// Created by Manuel on 05.02.2021.
//

#ifndef LAMA_DIFF_H
#define LAMA_DIFF_H

#include <string>
#include <typeinfo>
#include <unordered_map>
#include "../timestamp.h"
#include "../serializable.h"
#include "../../common/utils/json_utils.h"


enum DiffType {
    objectDiff,
    arrayDiff,
    valueDiff,
    arrayElemDiff
};

class diff : public serializable {

protected:

    struct base_params {
        DiffType _type;
        std::string _id;
        std::string _name;
        timestamp* _timestamp;
    };

    DiffType _diff_type;
    std::string _id;
    std::string _name;
    timestamp* _timestamp;

// constructor helpers
    static base_params extract_base_params_from_json(rapidjson::Value& json);
    static base_params create_base_params(std::string id, std::string name, DiffType type, timestamp* timestamp_ptr);

    // for deserialization
    static const std::unordered_map<std::string, DiffType> _string_to_diff_type;
    // for serialization
    static const std::unordered_map<DiffType, std::string> _diff_type_to_string;

// constructors

    diff(std::string var_name);     // base constructor
    diff(base_params params); // deserialization constructor
    diff(diff* other); // copy constructor

public:

// accessors
    timestamp* get_timestamp() const { return this->_timestamp; }
    std::string get_name() const { return this->_name; }
    std::string get_id() const { return this->_id; }
    DiffType get_type() const { return this->_diff_type; }

// diff interface
    bool merge(const diff* other);
    virtual bool merge_specialized(const diff* other) = 0;
    virtual bool has_changes() const = 0;
    virtual diff* clone() const = 0;

// serializable interface
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static diff* from_json(rapidjson::Value& json);

  //  template<class T>   // special version for templated diffs
   // value_diff<T>* value_diff_from_json(rapidjson::Value &json);   // TODO necessary?

};


#endif //LAMA_DIFF_H
