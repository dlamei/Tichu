#ifndef TICHU_REQUEST_HANDLER_H
#define TICHU_REQUEST_HANDLER_H

#include "../common/network/server_msg.h"
#include "../common/network/client_msg.h"

class request_handler {
public:
    static server_msg handle_request(const client_msg &req);
};

#endif //TICHU_REQUEST_HANDLER_H
