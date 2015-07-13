#include "sql_parser.h"

std::pair<bool, Program> parse_program(str_const_it begin, str_const_it end) {
    Program program;
    auto result_command = parse_command(begin, end);
    std::pair<bool, Query> result_query;
    if (result_command.first) {
        program.type = Program::Sql_command;
        program.command = result_command.second;
    }
    else {
        result_query = parse_query(begin, end);
        program.type = Program::Sql_query;
        program.query = result_query.second;
    }
    bool result = result_query.first || result_command.first;
    return std::make_pair(result, program);
}
