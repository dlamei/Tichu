#ifndef TICHU_SERIALIZABLE_H
#define TICHU_SERIALIZABLE_H

#include <memory>
#include <concepts>
#include <optional>
#include <utility>

#include "../../rapidjson/include/rapidjson/document.h"
//#include "value_type_helpers.h"

using json_value_ptr = std::shared_ptr<rapidjson::Value>;
using json_document_ptr = std::shared_ptr<rapidjson::Document>;

class serializable {
public:

    [[nodiscard]] virtual std::shared_ptr<rapidjson::Document> to_json() const {
        auto json = std::make_shared<rapidjson::Document>();
        json->SetObject();
        this->write_into_json(*json, json->GetAllocator());
        return json;
    }

    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const = 0;
};

template <typename T>
concept CanWriteToJson = requires(T t, rapidjson::Value val, rapidjson::Document::AllocatorType alloc) {
            { t.write_into_json(val, alloc) };
        };

template<typename T>
requires CanWriteToJson<T>
rapidjson::Value obj_to_json(const T &val, rapidjson::Document::AllocatorType &alloc) {
    rapidjson::Value json_val(rapidjson::kObjectType);
    val.write_into_json(json_val, alloc);
    return json_val;
}

rapidjson::Value int_to_json(int value, rapidjson::Document::AllocatorType &allocator);
rapidjson::Value bool_to_json(bool value, rapidjson::Document::AllocatorType &allocator);
rapidjson::Value string_to_json(const std::string &value, rapidjson::Document::AllocatorType &allocator);

template<typename T>
std::optional<T> primitive_from_json(const rapidjson::Value& json) {
    if (json.HasMember("value")) {
        return json["value"].Get<T>();
    }
    return {};
}

template<typename T>
static std::optional<T> primitive_from_json(const std::string &val_name, const rapidjson::Value& json) {
    if (!json.HasMember(val_name)) {
        return {};
    }

    auto json_value = json[val_name].GetObject();

    if (json_value.HasMember("value")) {
        return json_value["value"].Get<T>();
    }
    return {};
}

class UUID : serializable {
    std::string _id;

public:
    UUID() = default;

    explicit UUID(std::string id)
        : _id(std::move(id)) {}

    static UUID create();

    bool operator==(const UUID &other) const {
        return _id == other._id;
    };

    [[nodiscard]] bool empty() const { return _id.empty(); }
    [[nodiscard]] const char *c_str() const { return _id.c_str(); }
    [[nodiscard]] const std::string &string() const {return _id; }
    static std::optional<UUID> from_json(const rapidjson::Value &json);
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
};

template <>
struct std::hash<UUID>
{
    std::size_t operator()(const UUID& id) const
    {
        return std::hash<std::string>()(id.string());
    }
};



#endif //TICHU_SERIALIZABLE_H
