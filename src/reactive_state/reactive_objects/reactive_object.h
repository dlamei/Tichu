//
// Created by Manuel on 03.02.2021.
//

#ifndef LAMA_REACTIVE_OBJECT_H
#define LAMA_REACTIVE_OBJECT_H


#include "../diffs/diff.h"
#include "../timestamp.h"
#include "../serializable.h"
#include "../diffs/object_diff.h"

class reactive_object : public serializable {
protected:

    std::string _id;       // unique identifier
    std::string _name;     // name of the variable
    timestamp* _timestamp; // timestamp when this reactive_object was last updated

    struct base_params {
        std::string _id;
        std::string _name;
        timestamp* _timestamp;
    };

    static base_params extract_base_params(const rapidjson::Value& json);
    static base_params extract_base_params(const diff& full_diff);
    static base_params create_base_params(std::string id, std::string var_name, timestamp* timestamp_ptr);

    void add_base_params_to_full_diff(object_diff* full_diff);

    reactive_object(std::string var_name);
    reactive_object(base_params& params);
    reactive_object(std::string id, std::string var_name, timestamp* timestamp_ptr);

public:
// accessors
    std::string get_id() const;
    std::string get_name() const;
    timestamp* get_timestamp() const;

// reactive_object interface
    virtual ~reactive_object();

    bool apply_diff(const diff* diff);
    virtual diff* to_full_diff() const = 0;
    virtual bool apply_diff_specialized(const diff* diff) = 0;

// serializable interface
    virtual void write_into_json(rapidjson::Value& json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const override;

};

#endif //LAMA_REACTIVE_OBJECT_H
