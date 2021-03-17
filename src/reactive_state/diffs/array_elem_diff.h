//
// Created by Manuel on 16.02.2021.
//

#ifndef LAMA_ARRAY_ELEM_DIFF_H
#define LAMA_ARRAY_ELEM_DIFF_H

#include "diff.h"
#include "../../../rapidjson/include/rapidjson/document.h"


class array_elem_diff : public diff {
public:

    enum ModType {
        add, // new element added to array
        rem, // element removed from array
        mod  // element of array modified
    };

private:
    ModType _type;      // 'add', 'rem' or 'mod'
    int _idx;           // the position in the array where the add/rem/mod happened
    std::string _id;    // the id of the modified element
    diff* _elem;        // the changes to the element (only for 'add' and 'mod')

    // for deserialization
    static const std::unordered_map<std::string, ModType> _string_to_mod_type;
    // for serialization
    static const std::unordered_map<ModType, std::string> _mod_type_to_string;

    // deserialization constructor
    array_elem_diff(base_params params, ModType type, int idx, std::string id, diff* elem);

public:

    array_elem_diff(std::string name, ModType type, int idx, std::string id, diff* elem);

    bool equals(array_elem_diff* other) {
        return this->_type == other->_type && this->_id == other->_id;
    }

// Accessors
    ModType get_type() { return this->_type; }
    int get_index() { return this->_idx; }
    std::string get_elem_id() { return this->_id; }
    diff* get_diff() { return this->_elem; }

// Diff interface
    bool has_changes() const override;
    virtual bool merge_specialized(const diff* other) override;

    diff* clone() const override;

// Serializable interface
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

    static array_elem_diff* from_json(rapidjson::Value& json);
};

#endif //LAMA_ARRAY_ELEM_DIFF_H
