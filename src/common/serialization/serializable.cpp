#include "serializable.h"
#include <sstream>
#include <random>
#include <utility>

std::string generate_uuid_v4() {
    std::stringstream ss;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);

    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    };
    return ss.str();
}


void UUID::write_into_json(rapidjson::Value &json, rapidjson::MemoryPoolAllocator<> &allocator) const {
    rapidjson::Value id_val(_id.c_str(), allocator);
    json.AddMember("id", id_val, allocator);
}

std::optional<UUID> UUID::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id")) {
        return UUID(json["id"].GetString());
    } else {
        return {};
    }
}

UUID UUID::create() {
    return UUID(generate_uuid_v4());
}

rapidjson::Value int_to_json(int value, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
    rapidjson::Value json_val(rapidjson::kObjectType);
    json_val.AddMember("value", rapidjson::Value(value), allocator);
    return json_val;
}

rapidjson::Value bool_to_json(bool value, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
    rapidjson::Value json_val(rapidjson::kObjectType);
    json_val.AddMember("value", rapidjson::Value(value), allocator);
    return json_val;
}

rapidjson::Value
string_to_json(const std::string &value, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
    rapidjson::Value json_value(rapidjson::kObjectType);
    rapidjson::Value str_val;
    str_val.SetString(value.c_str(), allocator);
    str_val.AddMember("value", str_val, allocator);
    return json_value;
}

