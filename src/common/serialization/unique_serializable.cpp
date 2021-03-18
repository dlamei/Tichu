//
// Created by Manuel on 19.02.2021.
//

#include "reactive_object.h"

#include "../utils/uuid_generator.h"
#include "../utils/LamaException.h"


reactive_object::reactive_object()
    : _id(uuid_generator::generate_uuid_v4())
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
