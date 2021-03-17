//
// Created by Manuel on 02.03.2021.
//

#ifndef LAMA_GLOBAL_STATE_H
#define LAMA_GLOBAL_STATE_H

#include <string>

#include "../../rapidjson/include/rapidjson/writer.h"
#include "../../rapidjson/include/rapidjson/document.h"
#include "../../rapidjson/include/rapidjson/stringbuffer.h"

#include "../../game_state/game_state.h"


class global_state {
public:

    // TODO REMOVE
    inline static game_state* global_game_state = nullptr;
    inline static std::string global_game_id = "";
    // TODO END REMOVE
};

#endif //LAMA_GLOBAL_STATE_H
