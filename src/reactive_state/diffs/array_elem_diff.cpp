//
// Created by Manuel on 18.02.2021.
//

#include "array_elem_diff.h"

#include "../../common/utils/LamaException.h"

// for deserialization
const std::unordered_map<std::string, array_elem_diff::ModType> array_elem_diff::_string_to_mod_type = {
        {"mod", array_elem_diff::ModType::mod },
        {"rem", array_elem_diff::ModType::rem},
        {"add", array_elem_diff::ModType::add}
};
// for serialization
const std::unordered_map<array_elem_diff::ModType, std::string> array_elem_diff::_mod_type_to_string = {
        { array_elem_diff::ModType::mod, "mod" },
        { array_elem_diff::ModType::rem, "rem"},
        { array_elem_diff::ModType::add, "add"}
};

array_elem_diff::array_elem_diff(diff::base_params params, array_elem_diff::ModType type, int idx, std::string id,
                                 diff *elem) :
        diff(params),
        _type(type),
        _idx(idx),
        _id(id),
        _elem(elem)
{ }

array_elem_diff::array_elem_diff(std::string name, array_elem_diff::ModType type, int idx, std::string id, diff *elem) :
        diff(diff::create_base_params(id, name, DiffType::arrayElemDiff, new timestamp())),
        _type(type),
        _idx(idx),
        _id(id),
        _elem(elem)
{ }

bool array_elem_diff::merge_specialized(const diff *other) {
    const array_elem_diff* valid_other = dynamic_cast<const array_elem_diff*>(other);
    if (valid_other != nullptr) {
        if (valid_other->_timestamp->is_newer(this->_timestamp)) {
            if (_type == valid_other->_type && this->_id == valid_other->_id) {
                this->_idx = valid_other->_idx;
                if (valid_other->_elem != nullptr && this->_elem != nullptr) {
                    this->_elem->merge(valid_other);
                }
                return true;
            }
        }
    }
    return false;
}

diff *array_elem_diff::clone() const {
    diff* elem_clone = nullptr;
    if (_elem != nullptr) {
        elem_clone = _elem->clone();
    }
    std::string name_clone = _name;
    return new array_elem_diff(diff::create_base_params(_id, name_clone, _diff_type, _timestamp->clone()), _type, _idx, _id, elem_clone);
}

void array_elem_diff::write_into_json(rapidjson::Value &json,
                                      rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const  {
    diff::write_into_json(json, allocator);

    json.AddMember("idx", _idx, allocator);
    json.AddMember("id",rapidjson::Value (_id.c_str(), allocator) , allocator);
    json.AddMember("mod_type", rapidjson::Value (_mod_type_to_string.at(_type).c_str(), allocator), allocator);

    if (_elem != nullptr) {
        rapidjson::Value elem_val(rapidjson::kObjectType);
        _elem->write_into_json(elem_val, allocator);
        json.AddMember("elem", elem_val, allocator);
    }
}

array_elem_diff *array_elem_diff::from_json(rapidjson::Value &json) {
    if (json.HasMember("mod_type") && json.HasMember("idx") && json.HasMember("id") && json.HasMember("type")) {
        if (json["type"].GetString() == diff::_diff_type_to_string.at(DiffType::arrayElemDiff)) {
            diff* elem = nullptr;
            if (json.HasMember("elem")) {
                elem = diff::from_json(json["elem"].GetObject());
            }
            ModType type = _string_to_mod_type.at(json["mod_type"].GetString());
            return new array_elem_diff(diff::extract_base_params_from_json(json), type, json["idx"].GetInt(), json["id"].GetString(), elem);
        }
    }
    throw LamaException("Failed to create array_elem_diff from json.\n" + json_utils::to_string(&json));
}

bool array_elem_diff::has_changes() const {
    return true;
}
