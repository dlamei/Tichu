#ifndef TICHU_SERIALIZABLE_H
#define TICHU_SERIALIZABLE_H

#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include <iostream>

#include "../../rapidjson/include/rapidjson/document.h"
#include "../../common/exceptions/TichuException.h"

// we use 6 digits to encode the length of one message, this allows for reading the entire message with two read calls:
// one read for MESSAGE_SIZE_LENGTH bytes to get the length and the second with the received length
#define MESSAGE_SIZE_LENGTH 4
#define MAX_MESSAGE_SIZE 9999

using json_value_ptr = std::shared_ptr<rapidjson::Value>;
using json_document_ptr = std::shared_ptr<rapidjson::Document>;

struct JsonWriter {
    rapidjson::Value json;
    rapidjson::Document::AllocatorType alloc;
};


class json_utils {
public:
    static std::string to_string(const rapidjson::Value &json);
    static std::string to_pretty_string(const rapidjson::Value &json);

    // In case you need to create a rapidjson::Document on the heap (pointer) based on a value extracted from a json.
    static json_document_ptr clone_value(const rapidjson::Value &val);

};

template<class T, class B>
struct derived_from {
    static void constraints(T *p) { B *pb = p; }

    derived_from() { void (*p)(T *) = constraints; }
};

class serializable {
public:

    [[nodiscard]] virtual std::shared_ptr<rapidjson::Document> to_json() const {
        auto json = std::make_shared<rapidjson::Document>();
        json->SetObject();
        this->write_into_json(*json, json->GetAllocator());

        return json;
    }

    virtual void
    write_into_json_obj(const char *name, rapidjson::Value &json, rapidjson::Document::AllocatorType &alloc) const {
        rapidjson::Value json_val(rapidjson::kObjectType);
        this->write_into_json(json_val, alloc);
        json.AddMember(rapidjson::StringRef(name), json_val, alloc);
    }

    virtual void write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType &allocator) const = 0;
};

void int_into_json(const char *name, int value, rapidjson::Value &json,
                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator);

std::optional<int> int_from_json(const char *name, const rapidjson::Value &json);

void
bool_into_json(const char *name, bool value, rapidjson::Value &json, rapidjson::Document::AllocatorType &allocator);

std::optional<bool> bool_from_json(const char *name, const rapidjson::Value &json);

void string_into_json(const char *name, const std::string &value, rapidjson::Value &json,
                      rapidjson::Document::AllocatorType &alloc);

std::optional<std::string> string_from_json(const char *name, const rapidjson::Value &json);

template<typename T>
void vec_into_json(const char *name, const std::vector<T> &vec, rapidjson::Value &json,
                   rapidjson::Document::AllocatorType &alloc) {
    derived_from<T, serializable>(); // ensure T derives from serializable

    rapidjson::Value arr_val(rapidjson::kArrayType);
    for (const auto &element: vec) {
        rapidjson::Value val(rapidjson::kObjectType);
        element.write_into_json(val, alloc);
        arr_val.PushBack(val, alloc);
    }

    json.AddMember(rapidjson::StringRef(name), arr_val, alloc);
}

template<typename T>
std::optional<std::vector<T>> vec_from_json(const char *name, const rapidjson::Value &json) {
    derived_from<T, serializable>(); // ensure T derives from serializable

    if (!json.HasMember(name)) {
        return {};
    }

    std::vector<T> vec;
    for (auto &obj: json[name].GetArray()) {
        vec.push_back(T::from_json(obj.GetObject()));
    }

    return vec;
}

class UUID {
    std::string _id;

public:
    UUID() = default;

    explicit UUID(std::string id)
            : _id(std::move(id)) {}

    static UUID create();

    bool operator==(const UUID &other) const {
        return _id == other._id;
    };

    bool operator!=(const UUID &other) const {
        return !(*this == other);
    }

    [[nodiscard]] bool empty() const { return _id.empty(); }

    [[nodiscard]] const char *c_str() const { return _id.c_str(); }

    [[nodiscard]] const std::string &string() const { return _id; }
};

template<>
struct std::hash<UUID> {
    std::size_t operator()(const UUID &id) const {
        return std::hash<std::string>()(id.string());
    }
};


#endif //TICHU_SERIALIZABLE_H
