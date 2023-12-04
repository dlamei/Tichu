#ifndef TICHU_LISTENER_H
#define TICHU_LISTENER_H

#include <sockpp/stream_socket.h>
#include "utils.h"

// function for creating a tcp listener thread that pushes read messages in a queue
template <typename MSG>
void tcp_listener(sockpp::stream_socket socket,
                  const std::function<MSG(const std::string &)> &message_parser,
                  MessageQueue<MSG> *queue) {

    const int MSG_LEN_SIZE = sizeof(int) * 2; // we write the size as int as hexadecimal
    char msg_size_buff[MSG_LEN_SIZE];
    while (true) {
        socket.read_n(msg_size_buff, MSG_LEN_SIZE);
        if (!socket) break;

        int msg_size;
        try {
            msg_size = (int) std::stoul(msg_size_buff, nullptr, 16); // 16 for hexadecimal
        } catch (std::exception &e) {
            // maybe delimiter so we can try to recover, but since its tcp not sure if necessary
            ERROR("while trying to parse message size from string: {}", msg_size_buff);
            break;
        }

        // skip the ':' after the message size
        char c{};
        socket.read_n(&c, 1);
        if (!socket || c != ':') break;

        std::vector<char> buffer;
        buffer.resize(msg_size);
        socket.read_n(buffer.data(), msg_size);
        std::string msg_str = std::string(buffer.begin(), buffer.end());
        if (!socket) break;

        try {
            MSG msg = message_parser(msg_str);
            queue->push(msg);
        } catch (std::exception &e) {
            ERROR("{}\nwhile trying to parse: {}", e.what(), msg_str);
        }
    }

    if (!socket) {
        ERROR("socket error: {}", socket.last_error_str());
    }
    socket.shutdown();
}

#endif //TICHU_LISTENER_H
