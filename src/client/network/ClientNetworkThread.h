#ifndef LAMA_CLIENT_NETWORK_THREAD_H
#define LAMA_CLIENT_NETWORK_THREAD_H

#include <functional>
#include <wx/wx.h>
#include "sockpp/tcp_socket.h"
#include "sockpp/tcp_connector.h"
#include "../../network/requests/client_request.h"


class ClientNetworkThread : public wxThread {

public:
    ClientNetworkThread(const std::string& host, const uint16_t port);
    ~ClientNetworkThread();

    void sendRequest(const client_request& request);

    bool isConnected() const;

protected:
    virtual ExitCode Entry();

private:
    std::string _host;
    uint16_t _port;

    sockpp::tcp_connector _connection;
    sockpp::inet_address _address;

    bool _connectionSuccess;
    bool _failedToConnect;

    bool connect();
    void listen();

    void outputError(std::string title, std::string message);
    void setStatus(std::string message);

    void parseMessage(const std::string& message, const sockpp::tcp_socket::addr_t& socket);
};

#endif //LAMA_CLIENT_NETWORK_THREAD_H
