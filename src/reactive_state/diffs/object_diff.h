//
// Created by Manuel on 03.02.2021.
//

#ifndef LAMA_OBJECT_DIFF_H
#define LAMA_OBJECT_DIFF_H

#include <unordered_map>

#include "diff.h"

class object_diff : public diff {

private:

    std::unordered_map<std::string, diff*> _param_diffs;    // a map of all contained parameter diffs

    bool contains_param_name(const std::string& name) const;

    // deserialization constructor
    object_diff(base_params params, std::unordered_map<std::string, diff*> param_diffs);

    // copy constructor (for clone())
    explicit object_diff(object_diff* other);

public:

    object_diff(std::string id, std::string name);

// accessors
    virtual bool has_changes();

// serialization interface
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static object_diff* from_json(rapidjson::Value& json);

// diff interface
    virtual bool has_changes() const override;
    virtual bool merge_specialized(const diff* other) override;
    virtual diff* clone() const override;

// object_diff functions
    bool add_param_diff(const std::string& name, diff* param_diff);
    bool try_get_param_diff(const std::string& name, diff*& param_diff) const;
};

#endif //LAMA_OBJECT_DIFF_H
