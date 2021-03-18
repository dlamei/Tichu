//
// Created by Manuel on 19.02.2021.
//

#include "reactive_object.h"

#include "../../common/utils/uuid_generator.h"
#include "../../common/utils/LamaException.h"


reactive_object::reactive_object(std::string var_name)
    : _id(uuid_generator::generate_uuid_v4()),
    _name(var_name)
{ }

reactive_object::reactive_object(reactive_object::base_params &params)
    : _id(params._id),
    _name(params._name)
{ }

reactive_object::reactive_object(std::string id, std::string var_name)
    : _id(id),
    _name(var_name)
{ }

std::string reactive_object::get_id() const {
    return this->_id;
}

std::string reactive_object::get_name() const {
    return this->_name;
}

void reactive_object::write_into_json(rapidjson::Value &json,
                                      rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    rapidjson::Value id_val(_id.c_str(), allocator);
    json.AddMember("id", id_val, allocator);

    rapidjson::Value name_val(_name.c_str(), allocator);
    json.AddMember("name", name_val, allocator);

}

reactive_object::base_params reactive_object::extract_base_params(const rapidjson::Value &json)  {
    if (json.HasMember("id")) {
        base_params res;
        res._name = json["name"].GetString();
        res._id = json["id"].GetString();
        return res;
    } else {
        throw LamaException("Failed to deserialize reactive_object. Could not find timestamp or id");
    }
}

reactive_object::base_params reactive_object::create_base_params(std::string id, std::string var_name) {
    base_params res;
    res._name = var_name;
    res._id = id;
    return res;
}


