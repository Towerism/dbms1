#include "sql_command.h"
// Boost spirit grammars must be split up and divided
//   into many translation units so that the compiler
//   isn't overwhelmed by the entire grammar at once
std::pair<bool, Command> parse_insert_command(str_const_it begin, str_const_it end) {
    Sql_command_insert<str_const_it> grammar;

    Command command;
    bool result = qi::phrase_parse(begin, end, grammar, ascii::space, command);
    return std::make_pair(result && begin == end, command);
}
