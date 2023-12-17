#include "tichu.h"
#include <string>
#include <vector>

void parse_window_size(char **argv, ApplicationCreateInfo *parsed_args) {
    int pos_x = std::stoi(argv[0]);
    int pos_y = std::stoi(argv[1]);
    parsed_args->size = {pos_x, pos_y};
}

void parse_window_pos(char **argv, ApplicationCreateInfo *parsed_args) {
    int pos_x = std::stoi(argv[0]);
    int pos_y = std::stoi(argv[1]);
    parsed_args->position = {pos_x, pos_y};
}

void parse_player_name(char **argv, ApplicationCreateInfo *parsed_args) {
    std::string name = argv[0];
    parsed_args->player_name = name;
}

void parse_auto_connect(char **argv, ApplicationCreateInfo *parsed_args) {
    bool val = (int)std::stoi(argv[0]);
    parsed_args->auto_connect = val;
}

void print_help_msg(char **argv, ApplicationCreateInfo *parsed_args);

struct CmdOption {
    const char *opt_name;
    uint32_t n_args;
    void (*parse_func)(char **, ApplicationCreateInfo *);
    const char *desc;
};

std::vector<CmdOption> cmd_options {
        { "--help", 0, print_help_msg, "prints out this help message"},
        { "--pos", 2, parse_window_pos, "set the position of the application, default: empty"},
        { "--size", 2, parse_window_size, "set the size of the application, default: 1500 1000" },
        { "--name", 1, parse_player_name, "set the default name for the client, default: (empty)" },
        { "--auto", 1, parse_auto_connect, "set if the application should try to auto connect (can be buggy), default: 0" },
};

void print_help_msg(char **argv, ApplicationCreateInfo *parsed_args) {
    for (auto &opt: cmd_options) {
        std::cout << "\n" << opt.opt_name << ":\n";
        std::cout << opt.desc << "\n";
    }
}

ApplicationCreateInfo parse_cmd_args(int argc, char **argv) {
    ApplicationCreateInfo parsed_args{};
    int arg_indx = 1; // first arg is path
    while (arg_indx < argc) {
        bool found = false;
        for (auto &opt: cmd_options) {
            if (std::strcmp(argv[arg_indx], opt.opt_name) == 0) {
                found = true;
                if (arg_indx + opt.n_args >= argc) {
                    ERROR("false number of args for cmd option: {}", opt.opt_name);
                    return parsed_args;
                }
                arg_indx++;
                try {
                    opt.parse_func(argv + arg_indx, &parsed_args);
                } catch (std::exception &e) {
                    ERROR("while trying to parse option: {}\n{}", opt.opt_name, e.what());
                    return parsed_args;
                }
                arg_indx += (int)opt.n_args;
                break;
            }
        }
        if (!found) {
            ERROR("could not find option: {}", argv[arg_indx]);
        }
    }
    return parsed_args;
}

int main(int argc, char **argv) {
    auto info = parse_cmd_args(argc, argv);

    Application app(info);
    app.push_layer(std::make_shared<TichuGame>());
    app.run();

    return 0;
}