#include "sql_command.h"
// Boost spirit grammars must be split up and divided
//   into many translation units so that the compiler
//   isn't overwhelmed by the entire grammar at once
std::pair<bool, Command> parse_complex_command(str_const_it begin, str_const_it end) {
    auto result_command = parse_create_command(begin, end);
    if (!result_command.first) {
        result_command = parse_update_command(begin, end);
        if (!result_command.first) {
            result_command = parse_delete_command(begin, end);
            if (!result_command.first) {
                result_command = parse_insert_command(begin, end);
            }
        }
    }
    return result_command;
}
