//
// Created by Manuel on 05.02.2021.
//

#include "object_diff.h"

#include <iostream>

#include "../../common/utils/LamaException.h"
#include "../../common/utils/json_utils.h"


object_diff::object_diff(object_diff *other) : diff(other)  {
    for(auto it = other->_param_diffs.begin(); it != other->_param_diffs.end(); it++) {
        this->_param_diffs.insert({it->first, it->second->clone()});
    }
}

object_diff::object_diff(base_params params, std::unordered_map<std::string, diff *> param_diffs)
        : diff(params) ,
          _param_diffs(param_diffs)
{ }


object_diff::object_diff(std::string id, std::string name)
        :diff(diff::create_base_params(id, name, DiffType::objectDiff, new timestamp()))
{ }


bool object_diff::contains_param_name(const std::string& name) const {
    return _param_diffs.find(name) != _param_diffs.end();
}

bool object_diff::has_changes() {
    return _param_diffs.size() != 0;
}

bool object_diff::try_get_param_diff(const std::string& name, diff*& param_diff) const {
    param_diff = nullptr;
    if (this->contains_param_name(name)) {
        param_diff = _param_diffs.at(name);
        return true;
    } else {
        return false;
    }
}

bool object_diff::add_param_diff(const std::string& name, diff* param_diff) {
    diff* old_diff;
    if (this->try_get_param_diff(name, old_diff)) {
        return old_diff->merge(param_diff);
    } else {
        _param_diffs[name] = param_diff;
        return true;
    }
}

bool object_diff::has_changes() const {
    for (auto& elem : _param_diffs) {
        if (elem.second->has_changes()) {
            return true;
        }
    }
    return false;
}

bool object_diff::merge_specialized(const diff *other) {
    const object_diff* other_state_diff = dynamic_cast<const object_diff*>(other);
    if (other_state_diff != nullptr) {
        for (auto it = other_state_diff->_param_diffs.begin(); it != other_state_diff->_param_diffs.end(); it++) {
            if (this->contains_param_name(it->first)) {
                this->_param_diffs[it->first]->merge_specialized(it->second);
                // TODO update timestamps
            } else {
                this->add_param_diff(it->first, it->second->clone());
                // TODO update timestamps
            }
        }
        return true;
    }
    return false;
}

diff *object_diff::clone() const {
    rapidjson::Document doc(rapidjson::kObjectType);
    this->write_into_json(doc, doc.GetAllocator());
    try {
        return from_json(doc);
    } catch (std::exception e) {
        std::cerr << "Failed to clone object_diff" << std::endl
                  << "Content was : " << json_utils::to_string(&doc) << std::endl
                  << "Error was " << e.what() << std::endl;
        return nullptr;
    }
}


void object_diff::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    diff::write_into_json(json, allocator);
    rapidjson::Value arr_val(rapidjson::kArrayType);
    for(auto it = _param_diffs.begin(); it != _param_diffs.end(); it++) {
        rapidjson::Value arr_elem(rapidjson::kObjectType);
        it->second->write_into_json(arr_elem, allocator);
        arr_val.PushBack(arr_elem, allocator);
    }
    json.AddMember("params", arr_val, allocator);
}

object_diff *object_diff::from_json(rapidjson::Value& json) {
    if (json.HasMember("type") && json.HasMember("params")) {
        if (json["type"].GetString() == diff::_diff_type_to_string.at(DiffType::objectDiff)) {
            std::unordered_map<std::string, diff*> parameters;
            for (auto& serialized_elem : json["params"].GetArray()) {
                diff* param = diff::from_json(serialized_elem);
                parameters.insert({param->get_name(), param});
            }
            return new object_diff(diff::extract_base_params_from_json(json), parameters);
        }
    }
    throw LamaException("Object to deserialize was not of type object_diff. Object was\n" + json_utils::to_string(&json));
}

