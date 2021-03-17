//
// Created by Manuel on 12.02.2021.
//

#ifndef LAMA_ARRAY_DIFF_H
#define LAMA_ARRAY_DIFF_H

#include "diff.h"

#include <vector>

#include "array_elem_diff.h"
#include "../diffable_utils.h"
#include "../serializable.h"
#include "../../common/utils/LamaException.h"




class array_diff : public diff {
private:

    std::vector<array_elem_diff*> _operations;

    array_diff(base_params params, std::vector<array_elem_diff*> operations) :
        diff(params),
        _operations(operations)
    { }

    bool add_operation(array_elem_diff* elem) {
        _operations.push_back(elem);
        return true;
    }

public:

    array_diff(std::string id, std::string name) :  diff(diff::create_base_params(id, name, DiffType::arrayDiff, new timestamp())) {}
    ~array_diff() {
        for (int i = 0; i < _operations.size(); i++) {
            delete _operations.at(i);
        }
        _operations.clear();
    }

// diff interface
    bool has_changes() const override {
        return this->_operations.size() > 0;
    }

    bool merge_specialized(const diff *other) override {
        const array_diff* valid_diff = dynamic_cast<const array_diff*>(other);
        if (valid_diff != nullptr) {
            if (this->_operations.size() == 0) {
                for (int i_other = 0; i_other < valid_diff->_operations.size(); i_other++) {
                    this->_operations.emplace_back(dynamic_cast<array_elem_diff*>(valid_diff->_operations.at(i_other)->clone()));
                }
                // TODO update timestamps
            } else if (valid_diff->_operations.size() > 0){
                int i_this = 0;
                int i_other = 0;
                array_elem_diff* this_elem = this->_operations.at(0);
                array_elem_diff* other_elem = valid_diff->_operations.at(0);
                while (i_other < valid_diff->_operations.size() && i_this < this->_operations.size()) {
                    if (other_elem->get_timestamp()->is_newer(this_elem->get_timestamp())) {
                        i_this++;
                        if (i_this < this->_operations.size()) {
                            this_elem = this->_operations.at(i_this);
                        }
                    } else {
                        if (this_elem->equals(other_elem)) {
                            this_elem->merge_specialized(other_elem);
                        } else {
                            this->_operations.insert(_operations.begin() + i_this, dynamic_cast<array_elem_diff*>(other_elem->clone()));
                            i_this++;   // same element but moved by one position
                        }

                        i_other++;
                        if (i_other < valid_diff->_operations.size()) {
                            other_elem = valid_diff->_operations.at(i_this);
                        }
                    }
                }
                // If there are still elements remaining in the other diff, then they all have a newer timestamp. Add them
                while (i_other < valid_diff->_operations.size()) {
                    this->_operations.push_back(dynamic_cast<array_elem_diff*>(other_elem->clone()));
                    i_other++;
                    if (i_other < valid_diff->_operations.size()) {
                        other_elem = valid_diff->_operations.at(i_this);
                    }
                }
            }
            return true;
        }
        return false;
    }

    virtual diff* clone() const override {
        rapidjson::Document* json = this->to_json();
        diff* res = from_json(*json);
        delete json;
        return res;
    }

// array_diff functions
    void add_removal(int idx, std::string id) {
        add_operation(new array_elem_diff(id, array_elem_diff::ModType::rem, idx, id, nullptr));
    }

    void add_insertion(int idx, std::string id, diff* full_diff) {
        add_operation(new array_elem_diff(id, array_elem_diff::ModType::add, idx, id, full_diff));
    }

    void add_modification(int idx, std::string id, diff* diff) {
        add_operation(new array_elem_diff(id, array_elem_diff::ModType::mod, idx, id, diff));
    }

    const std::vector<array_elem_diff*> get_diffs() const {
        return _operations;
    }

// serializable interface
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override {
        diff::write_into_json(json, allocator);
        json.AddMember("arr", diffable_utils::serialize_vector(_operations, allocator), allocator);
    }

    static array_diff* from_json(rapidjson::Value& json) {
        if (json.HasMember("type") && json.HasMember("arr")) {
            if (json["type"].GetString() == diff::_diff_type_to_string.at(DiffType::arrayDiff)) {
                std::vector<array_elem_diff*> operations;
                for (auto& serialized_elem : json["arr"].GetArray()) {
                    operations.push_back(array_elem_diff::from_json(serialized_elem));
                }
                return new array_diff(diff::extract_base_params_from_json(json), operations);
            }
        }
        throw LamaException("Failed to create array_diff from json.\n" + json_utils::to_string(&json));
    }
};




#endif //LAMA_ARRAY_DIFF_H
