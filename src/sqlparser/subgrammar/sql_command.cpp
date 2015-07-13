#include "sql_command.h"

std::pair<bool, Command> parse_command(str_const_it begin, str_const_it end) {
    auto result_command = parse_simple_command(begin, end);
    if (!result_command.first) {
        result_command = parse_complex_command(begin, end);
        if (!result_command.first) {
        }
    }
    return result_command;
}

