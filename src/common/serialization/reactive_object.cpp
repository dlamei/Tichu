//
// Created by Manuel on 19.02.2021.
//

#include "reactive_object.h"

#include "../utils/uuid_generator.h"
#include "../utils/LamaException.h"


reactive_object::reactive_object()
    : _id(uuid_generator::generate_uuid_v4())
{ }

reactive_object::reactive_object(reactive_object::base_params &params)
    : _id(params._id)
{ }

reactive_object::reactive_object(std::string id)
    : _id(id)
{ }

std::string reactive_object::get_id() const {
    return this->_id;
}

void reactive_object::write_into_json(rapidjson::Value &json,
                                      rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    rapidjson::Value id_val(_id.c_str(), allocator);
    json.AddMember("id", id_val, allocator);

}

reactive_object::base_params reactive_object::extract_base_params(const rapidjson::Value &json)  {
    if (json.HasMember("id")) {
        base_params res;
        res._id = json["id"].GetString();
        return res;
    } else {
        throw LamaException("Failed to deserialize reactive_object. Could not find timestamp or id");
    }
}

reactive_object::base_params reactive_object::create_base_params(std::string id) {
    base_params res;
    res._id = id;
    return res;
}


