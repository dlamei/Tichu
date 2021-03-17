//
// Created by Manuel on 19.02.2021.
//

#include "reactive_object.h"

#include "../diffs/value_diff.h"
#include "../../common/utils/uuid_generator.h"
#include "../../common/utils/LamaException.h"


reactive_object::reactive_object(std::string var_name)
    : _id(uuid_generator::generate_uuid_v4()),
    _timestamp(new timestamp()),
    _name(var_name)
{ }

reactive_object::reactive_object(reactive_object::base_params &params)
    : _id(params._id),
    _name(params._name),
    _timestamp(params._timestamp)
{ }

reactive_object::reactive_object(std::string id, std::string var_name, timestamp *timestamp_ptr)
    : _id(id),
    _name(var_name),
    _timestamp(timestamp_ptr)
{ }

std::string reactive_object::get_id() const {
    return this->_id;
}

std::string reactive_object::get_name() const {
    return this->_name;
}

timestamp *reactive_object::get_timestamp() const {
    return this->_timestamp;
}

reactive_object::~reactive_object()  {
    if (_timestamp != nullptr) {
        delete _timestamp;
        _timestamp = nullptr;
    }
}

bool reactive_object::apply_diff(const diff *obj_diff)  {
    //const object_diff* valid_diff = dynamic_cast<const object_diff*>(diff);
    // TODO id? timestamp? name?
    return apply_diff_specialized(obj_diff);
}

void reactive_object::write_into_json(rapidjson::Value &json,
                                      rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    rapidjson::Value id_val(_id.c_str(), allocator);
    json.AddMember("id", id_val, allocator);

    rapidjson::Value name_val(_name.c_str(), allocator);
    json.AddMember("name", name_val, allocator);

    // No need for timestamp atm
    /* rapidjson::Value timestamp_val(rapidjson::kObjectType);
   _timestamp->write_into_json(timestamp_val, allocator);
   json.AddMember("timestamp", timestamp_val, allocator);*/
}

void reactive_object::add_base_params_to_full_diff(object_diff *full_diff) {
   /* full_diff->add_param_diff("name", new value_diff<std::string>(_id + "_name", "name", _name));
    full_diff->add_param_diff("id", new value_diff<std::string>(_id + "_id", "id", _id));
    full_diff->add_param_diff("timestamp", new value_diff<std::string>(_id + "_timestamp", "timestamp", _timestamp.));*/
}

reactive_object::base_params reactive_object::extract_base_params(const rapidjson::Value &json)  {
    if (/*json.HasMember("timestamp") &&*/ json.HasMember("id")) {
        base_params res;
        //res._timestamp = timestamp::from_json(json["timestamp"].GetObject());
        res._timestamp = new timestamp();
        res._name = json["name"].GetString();
        res._id = json["id"].GetString();
        return res;
    } else {
        throw LamaException("Failed to deserialize reactive_object. Could not find timestamp or id");
    }
}

reactive_object::base_params reactive_object::extract_base_params(const diff& full_diff) {
    base_params res;
    res._timestamp = full_diff.get_timestamp()->clone();
    res._name = full_diff.get_name();
    res._id = full_diff.get_id();
    return res;
}

reactive_object::base_params reactive_object::create_base_params(std::string id, std::string var_name, timestamp *ts) {
    base_params res;
    res._timestamp = ts;
    res._name = var_name;
    res._id = id;
    return res;
}


