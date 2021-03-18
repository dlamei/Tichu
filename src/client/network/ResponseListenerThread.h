#ifndef LAMA_CLIENT_NETWORK_THREAD_H
#define LAMA_CLIENT_NETWORK_THREAD_H

#include <functional>
#include <wx/wx.h>
#include "sockpp/tcp_socket.h"
#include "sockpp/tcp_connector.h"
#include "../../network/requests/client_request.h"


class ResponseListenerThread : public wxThread {

public:
    ResponseListenerThread(sockpp::tcp_connector* connection);
    ~ResponseListenerThread();

protected:
    virtual ExitCode Entry();

private:
    void outputError(std::string title, std::string message);


    sockpp::tcp_connector* _connection;

};

#endif //LAMA_CLIENT_NETWORK_THREAD_H
