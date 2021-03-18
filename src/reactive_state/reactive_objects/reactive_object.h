//
// Created by Manuel on 03.02.2021.
//

#ifndef LAMA_REACTIVE_OBJECT_H
#define LAMA_REACTIVE_OBJECT_H


#include "../serializable.h"

class reactive_object : public serializable {
protected:

    std::string _id;       // unique identifier
    std::string _name;     // name of the variable

    struct base_params {
        std::string _id;
        std::string _name;
    };

    static base_params extract_base_params(const rapidjson::Value& json);
    static base_params create_base_params(std::string id, std::string var_name);

    reactive_object(std::string var_name);
    reactive_object(base_params& params);
    reactive_object(std::string id, std::string var_name);

public:
// accessors
    std::string get_id() const;
    std::string get_name() const;

// serializable interface
    virtual void write_into_json(rapidjson::Value& json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const override;

};

#endif //LAMA_REACTIVE_OBJECT_H
