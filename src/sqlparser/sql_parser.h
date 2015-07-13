#ifndef SQL_PARSER_H
#define SQL_PARSER_H

#include "subgrammar/sql_command.h"
std::pair<bool, Program> parse_program(str_const_it begin, str_const_it end);
#endif // SQL_PARSER_H
