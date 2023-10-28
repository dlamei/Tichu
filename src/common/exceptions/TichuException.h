//
// Created by Manuel on 09.03.2021.
//

#ifndef TICHU_TICHUEXCEPTION_H
#define TICHU_TICHUEXCEPTION_H

#include <string>

class TichuException : public std::exception {
private:
    std::string _msg;
public:
    explicit TichuException(const std::string& message) : _msg(message) { };

    const char* what() const noexcept override {
        return _msg.c_str();
    }
};

#endif //TICHU_TICHUEXCEPTION_H
