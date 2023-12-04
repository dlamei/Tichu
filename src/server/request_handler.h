#ifndef TICHU_REQUEST_HANDLER_H
#define TICHU_REQUEST_HANDLER_H

#include "../common/Messages.h"

namespace request_handler {
    ServerMsg handle_request(const ClientMsg &req);
}

#endif //TICHU_REQUEST_HANDLER_H
