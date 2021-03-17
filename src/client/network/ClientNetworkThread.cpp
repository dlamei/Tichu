#include "ClientNetworkThread.h"


#include <iostream>
#include <string>
#include <sockpp/exception.h>
#include "../GameController.h"

#include "../../network/responses/server_response.h"


ClientNetworkThread::ClientNetworkThread(const std::string& host, const uint16_t port) {
    this->_host = host;
    this->_port = port;

    this->_connectionSuccess = false;
    this->_failedToConnect = false;
}

ClientNetworkThread::~ClientNetworkThread() {
    this->_connection.shutdown();
}


wxThread::ExitCode ClientNetworkThread::Entry() {

    // initialize sockpp framework
    sockpp::socket_initializer sockInit;

    this->_connectionSuccess = false;
    this->_failedToConnect = false;

    if(this->connect()) {
        this->setStatus("Connected to " + _host + ":" + std::to_string(_port));
        this->_connectionSuccess = true;
        this->listen();

    } else {
        this->_failedToConnect = true;
        this->setStatus("Not connected");
    }

    return (wxThread::ExitCode) 0; // everything okay
}


bool ClientNetworkThread::connect() {

    try {
        this->_address = sockpp::inet_address(this->_host, this->_port);
    } catch (const sockpp::getaddrinfo_error& e) {
        this->outputError("Connection error", "Failed to resolve address " + e.hostname());
        return false;
    }

    this->setStatus("Opening connection to " + this->_address.to_string() + "...");

    if (!this->_connection.connect(this->_address)) {
        this->outputError("Connection error", "Failed to connect to server " + this->_host + ":" + std::to_string(this->_port));
        return false;
    }

    return true;
}


void ClientNetworkThread::listen() {
    try {
        char buffer[512]; // 512 bytes
        ssize_t count = 0;

        while ((count = this->_connection.read(buffer, sizeof(buffer))) > 0) {
            try {
                int pos = 0;

                // extract length of message in bytes (which is sent at the start of the message, and is separated by a ":")
                std::stringstream messageLengthStream;
                while (buffer[pos] != ':' && pos < count) {
                    messageLengthStream << buffer[pos];
                    pos++;
                }
                ssize_t messageLength = std::stoi(messageLengthStream.str());

                // initialize a stream for the message
                std::stringstream messageStream;

                // copy everything following the message length declaration into a stringstream
                messageStream.write(&buffer[pos + 1], count - (pos + 1));
                ssize_t bytesReadSoFar = count - (pos + 1);

                // read remaining packages until full message length is reached
                while (bytesReadSoFar < messageLength && count != 0) {
                    count = this->_connection.read(buffer, sizeof(buffer));
                    messageStream.write(buffer, count);
                    bytesReadSoFar += count;
                }

                // process message (if we've received entire message)
                if (bytesReadSoFar == messageLength) {
                    std::string message = messageStream.str();
                    GameController::getMainThreadEventHandler()->CallAfter([this, message]{
                        this->parseMessage(message, this->_connection.peer_address());
                    });

                } else {
                    this->outputError("Network error", "Could not read entire message. TCP stream ended early. Difference is " + std::to_string(messageLength - bytesReadSoFar) + " bytes");
                }

            } catch (std::exception& e) {
                // Make sure the connection isn't terminated only because of a read error
                this->outputError("Network error", "Error while reading message: " + (std::string) e.what());
            }
        }

        if (count <= 0) {
            this->outputError("Network error", "Read error [" + std::to_string(this->_connection.last_error()) + "]: " + this->_connection.last_error_str());
        }

    } catch(const std::exception& e) {
        this->outputError("Network error", "Error in listener thread: " + (std::string) e.what());
    }

    this->_connection.shutdown();
}


void ClientNetworkThread::sendRequest(const client_request &request) {

    // wait until network is connected (max. 5 seconds)
    int connectionCheckCounter = 0;
    while(!this->_connectionSuccess && !this->_failedToConnect && connectionCheckCounter < 200) {
        wxMilliSleep(25);
        connectionCheckCounter++;
    }

    // do not continue if failed to connect to server
    if(this->_failedToConnect) {
        return;
    }

    if(this->_connectionSuccess && this->_connection.is_connected()) {

        // serialize request into JSON string
        rapidjson::Document* jsonDocument = request.to_json();
        std::string message = json_utils::to_string(jsonDocument);
        delete jsonDocument;

        // turn message into stream and prepend message length
        std::stringstream messageStream;
        messageStream << std::to_string(message.size()) << ':' << message;
        message = messageStream.str();

        // send message to server
        ssize_t bytesSent = this->_connection.write(message);

        // if the number of bytes sent does not match the length of the message, probably something went wrong
        if (bytesSent != ssize_t(message.length())) {
            this->outputError("Network error", "Error writing to the TCP stream: " + this->_connection.last_error_str());
        }

    } else {
        this->outputError("Network error", "Lost connection to server");
    }
}


bool ClientNetworkThread::isConnected() const {
    return this->_connection.is_connected();
}


void ClientNetworkThread::outputError(std::string title, std::string message) {
    GameController::getMainThreadEventHandler()->CallAfter([title, message]{
        GameController::showError(title, message);
    });
}


void ClientNetworkThread::setStatus(std::string message) {
    GameController::getMainThreadEventHandler()->CallAfter([message]{
        GameController::showStatus(message);
    });
}


void ClientNetworkThread::parseMessage(const std::string& message, const sockpp::inet_address& socket) {

    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse(message.c_str());

    try {
        server_response* res = server_response::from_json(json);
        res->Process();

    } catch (std::exception e) {
        this->outputError("JSON parsing error", "Failed to parse message from server:\n" + message + "\n" + (std::string) e.what());
    }
}
