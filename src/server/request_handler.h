#ifndef TICHU_REQUEST_HANDLER_H
#define TICHU_REQUEST_HANDLER_H

#include "../common/messages.h"

namespace request_handler {
    std::optional<ServerMsg> handle_request(const ClientMsg &req);
}

#endif //TICHU_REQUEST_HANDLER_H
