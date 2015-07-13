#include "sql_query.h"

std::pair<bool, Query> parse_query(str_const_it begin, str_const_it end) {
    Sql_query<str_const_it> grammar;

    Query query;
    bool result = qi::phrase_parse(begin, end, grammar, ascii::space, query);
    return std::make_pair(result && begin == end, query);
}
