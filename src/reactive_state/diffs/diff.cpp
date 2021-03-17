//
// Created by Manuel on 17.02.2021.
//

#include "diff.h"

#include <iostream>

#include "value_diff.h"
#include "object_diff.h"
#include "array_diff.h"
#include "../value_type_helpers.h"
#include "../../common/utils/uuid_generator.h"
#include "../../common/utils/LamaException.h"

const std::unordered_map<std::string, DiffType> diff::_string_to_diff_type = {
        {"objectDiff", DiffType::objectDiff },
        {"arrayDiff", DiffType::arrayDiff},
        {"valueDiff", DiffType::valueDiff},
        {"arrayElemDiff", DiffType::arrayElemDiff}
};

const std::unordered_map<DiffType, std::string> diff::_diff_type_to_string = {
        {DiffType::objectDiff,    "objectDiff" },
        {DiffType::arrayDiff,     "arrayDiff" },
        {DiffType::valueDiff,     "valueDiff" },
        {DiffType::arrayElemDiff, "arrayElemDiff" }
};


diff::diff(std::string var_name)  : _id(uuid_generator::generate_uuid_v4()), _name(var_name), _timestamp(new timestamp()) {}

diff::diff(diff::base_params params)
        : _id(params._id),
          _diff_type(params._type),
          _name(params._name),
          _timestamp(params._timestamp)
{ }

diff::diff(diff *other)
        : _id(other->_id),
          _diff_type(other->_diff_type),
          _name(other->_name),
          _timestamp(other->_timestamp->clone())
{ }

diff::base_params diff::create_base_params(std::string id, std::string name, DiffType type, timestamp *timestamp_ptr)  {
    base_params res;
    res._id = id;
    res._type = type;
    res._name = name;
    res._timestamp = timestamp_ptr;
    return res;
}

bool diff::merge(const diff *other)  {
    if (typeid(*other) == typeid(*this)) {
        return merge_specialized(other);
    }
    return false;
}

void diff::write_into_json(rapidjson::Value &json,
                           rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const  {
    rapidjson::Value name_val(_name.c_str(), allocator);  // strings have to be allocated like this
    json.AddMember("name", name_val, allocator);

    rapidjson::Value id_val(_id.c_str(), allocator);  // strings have to be allocated like this
    json.AddMember("id", id_val, allocator);

    rapidjson::Value type_val(_diff_type_to_string.at(_diff_type).c_str(), allocator);  // strings have to be allocated like this
    json.AddMember("type", type_val, allocator);

    // No need to submit the timestamp atm
  /*  rapidjson::Value timestamp_val(rapidjson::kObjectType);
    _timestamp->write_into_json(timestamp_val, allocator);
    json.AddMember("timestamp", timestamp_val, allocator);*/
}

diff::base_params diff::extract_base_params_from_json(rapidjson::Value &json) {
    if (json.HasMember("name")/* && json.HasMember("timestamp")*/ && json.HasMember("id")  && json.HasMember("type")) {
        // timestamp* timestamp = timestamp::from_json(json["timestamp"].GetObject());
        timestamp* ts = new timestamp();
        return create_base_params(json["id"].GetString(), json["name"].GetString(), _string_to_diff_type.at(json["type"].GetString()), ts);
    } else {
        throw LamaException("Could not extract diff base parameters. JSON was: " + json_utils::to_string(&json));
    }
}
/*
template<class T>
value_diff<T>* diff::value_diff_from_json(rapidjson::Value &json)  {
    if (json.HasMember("type")) {
        std::string type = json["type"].GetString();
        DiffType diff_type = diff::_string_to_diff_type.at(type);

        if (diff_type == DiffType::valueDiff) {
             return value_diff<T>::from_json(json);
        }
        else {
            throw LamaException(("Encountered unknown DiffType " + type).c_str());
        }
    }
    throw LamaException(("Could not determine type of diff. JSON was:\n" + json_utils::to_string(&json)).c_str());
}
*/
diff *diff::from_json(rapidjson::Value &json)  {
    if (json.HasMember("type")) {
        DiffType diff_type = diff::_string_to_diff_type.at(json["type"].GetString());

        if (diff_type == DiffType::objectDiff) {
            return object_diff::from_json(json);
        }
        else if (diff_type == DiffType::valueDiff) {
            if (json.HasMember("value_type")) {
                value_type_helpers::ValueType value_type = value_type_helpers::_string_to_value_type.at(json["value_type"].GetString());
                switch (value_type) {
                    case (value_type_helpers::ValueType::boolType): {
                        return value_diff<bool>::from_json(json);
                    }
                    case (value_type_helpers::ValueType::intType): {
                        return value_diff<int>::from_json(json);
                    }
                    case (value_type_helpers::ValueType::uintType): {
                        return value_diff<unsigned int>::from_json(json);
                    }
                    case (value_type_helpers::ValueType::floatType): {
                        return value_diff<float>::from_json(json);
                    }
                    case (value_type_helpers::ValueType::doubleType): {
                        return value_diff<double>::from_json(json);
                    }
                    case (value_type_helpers::ValueType::int64tType): {
                        return value_diff<int64_t>::from_json(json);
                    }
                    case (value_type_helpers::ValueType::uint64tType): {
                        return value_diff<uint64_t>::from_json(json);
                    }
                    case (value_type_helpers::ValueType::stringType): {
                        return value_diff<std::string>::from_json(json);
                    }
                    default: {
                        throw LamaException("Could not deserialize value_diff. Unknown value_type " + std::string(json["value_type"].GetString()));
                    }
                }
            } else {
                throw LamaException("Failed to deserialize value_diff. Failed to extract value_type " + json_utils::to_string(&json));
            }
        }
        else if (diff_type == DiffType::arrayElemDiff) {
            return array_elem_diff::from_json(json);
        }
        else if (diff_type == DiffType::arrayDiff) {
            return array_diff::from_json(json);
        }
        else {
            throw LamaException("Failed to deserialize diff. Encountered unknown DiffType "
                                          + std::string(json["type"].GetString())
                                          + "\nJSON was " + std::string(json_utils::to_string(&json)));
        }
    } else {
        std::cout << "Could not find type" << std::endl;
    }
   std::cout << json_utils::to_string(&json) << std::endl;
    throw LamaException("Could not determine type of diff. JSON was:\n" + json_utils::to_string(&json));
}

