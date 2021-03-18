//
// Created by Manuel on 03.02.2021.
//

#ifndef LAMA_REACTIVE_OBJECT_H
#define LAMA_REACTIVE_OBJECT_H


#include "serializable.h"

class reactive_object : public serializable {
protected:

    std::string _id;       // unique identifier

    reactive_object();
    reactive_object(std::string id);

public:
// accessors
    std::string get_id() const;

// serializable interface
    virtual void write_into_json(rapidjson::Value& json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const override;

};

#endif //LAMA_REACTIVE_OBJECT_H
