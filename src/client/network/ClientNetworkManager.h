#ifndef TICHU_CLIENTNETWORKMANAGER_H
#define TICHU_CLIENTNETWORKMANAGER_H


#include <string>
#include "ResponseListenerThread.h"
#include "../../common/network/client_msg.h"


class ClientNetworkManager{

        public:
        static void init(const std::string& host, const uint16_t port);

        static void sendRequest(const client_msg& request);

        static void parseResponse(const std::string& message);

        private:
        static bool connect(const std::string& host, const uint16_t port);


        static sockpp::tcp_connector* _connection;

        static bool _connectionSuccess;
        static bool _failedToConnect;

};


#endif //TICHU_CLIENTNETWORKMANAGER_H
