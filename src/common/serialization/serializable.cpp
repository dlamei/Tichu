#include "serializable.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
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
	}
	return ss.str();
}


UUID UUID::create() {
	return UUID(generate_uuid_v4());
}

void int_into_json(const char *name, int value, rapidjson::Value &json, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
	json.AddMember(rapidjson::StringRef(name), rapidjson::Value(value), allocator);
}

std::optional<int> int_from_json(const char *name, const rapidjson::Value &json) {
	if (json.HasMember(name)) {
		return json[name].GetInt();
	}
	return {};
}

void bool_into_json(const char *name, bool value, rapidjson::Value &json, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
	json.AddMember(rapidjson::StringRef(name), rapidjson::Value(value), allocator);
}

std::optional<bool> bool_from_json(const char *name, const rapidjson::Value &json) {
	if (json.HasMember(name)) {
		return json[name].Get<bool>();
	}
	return {};
}


void
string_into_json(const char *name, const std::string &value, rapidjson::Value &json, rapidjson::Document::AllocatorType &alloc) {
	rapidjson::Value val(value.c_str(), alloc);
	json.AddMember(rapidjson::StringRef(name), val, alloc);
}

std::optional<std::string> string_from_json(const char *name, const rapidjson::Value &json) {
	if (json.HasMember(name)) {
		return json[name].GetString();
	}
	return {};
}


std::string json_utils::to_string(const rapidjson::Value &json) {
	rapidjson::StringBuffer buffer;
	buffer.Clear();
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	json.Accept(writer);
	return buffer.GetString();
}

json_document_ptr json_utils::clone_value(const rapidjson::Value &val) {
	json_document_ptr state_json = std::make_shared<rapidjson::Document>(rapidjson::Document(rapidjson::kObjectType));
	state_json->CopyFrom(val, state_json->GetAllocator());
	return state_json;
}
