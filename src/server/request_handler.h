#ifndef TICHU_REQUEST_HANDLER_H
#define TICHU_REQUEST_HANDLER_H

#include "../common/network/ServerMsg.h"
#include "../common/network/ClientMsg.h"

class request_handler {
public:
    static ServerMsg handle_request(const ClientMsg &req);
};

#endif //TICHU_REQUEST_HANDLER_H
