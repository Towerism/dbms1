#ifndef SQL_BASIC_H
#define SQL_BASIC_H

#include "sql_semantics.h"

using qi::int_;
using qi::char_;
using qi::alnum;
using qi::lit;
using qi::lexeme;
using qi::_val;
using qi::_a;
using qi::_1;
using phoenix::at_c;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace spirit = boost::spirit;
namespace phoenix = boost::phoenix;

template <typename Iterator> struct Sql_query;
template <typename Iterator> struct Sql_command_clause;
template <typename Iterator> struct Sql_command_simple;
template <typename Iterator> struct Sql_command_create;
template <typename Iterator> struct Sql_command_update;
template <typename Iterator> struct Sql_command_delete;
template <typename Iterator> struct Sql_command_insert;

template <typename Iterator>
struct Sql_basic: qi::grammar<Iterator, ascii::space_type> {
    Sql_basic() : Sql_basic::base_type(qi::eps) {
        identifier %= lexeme[char_("_a-zA-Z") >> *char_("_a-zA-Z0-9")];
        relation_name %= identifier;
        attribute_name %= identifier;
        attribute_list %= attribute_name % ',';

        int_literal = int_ [at_c<0>(_val) = Literal::Integer, at_c<1>(_val) = _1];
        string_literal = lexeme[
            lit("\"")       [at_c<0>(_val) = Literal::Var_char, at_c<1>(_val) = std::string("")]
            >> *(char_ -'"') [string_literal_builder(_val, _1)]
            >> "\""
        ];

        literal %= string_literal | int_literal;
        operand %= identifier | literal;
        compare %= 
            operand
            >> compare_op
            >> operand
            ;
        condition =
            conjunction       [_val = _1]
            >> *("||"
                >> conjunction [at_c<0>(_a) = _val, at_c<1>(_a) = Condition::Or, at_c<2>(_a) = _1, _val = _a]
                )
            ;
        conjunction =
            atomic_condition       [_val = _1]
            >> *("&&"
                >> atomic_condition [at_c<0>(_a) = _val, at_c<1>(_a) = Condition::And, at_c<2>(_a) = _1, _val = _a]
                )
            ;
        atomic_condition =
            compare            [at_c<0>(_val) = _1]
            | condition_clause [_val = _1]
            ;
        typed_attribute_list %= (attribute_name >> type) % ',';
        assignment_list %= assignment % ',';
        assignment %= attribute_name >> "=" >> literal;
        type = 
            (qi::string("VARCHAR")        [at_c<0>(_val) = _1]
                >> varchar_size_clause [at_c<1>(_val) = _1]
                ) 
            | qi::string("INTEGER")       [at_c<0>(_val) = _1]
            ;

        attr_list_clause %=
            '('
            >> attribute_list
            >> ')'
            ;
        varchar_size_clause %=
            '('
            >> int_
            >> ')'
            ;
        condition_clause %=
            '('
            >> condition
            >> ')'
            ;
    }
private:
    qi::rule<Iterator, std::string(), ascii::space_type> relation_name;
    qi::rule<Iterator, std::string(), ascii::space_type> identifier;
    qi::rule<Iterator, boost::variant<std::string, Literal>(), ascii::space_type> operand;
    qi::rule<Iterator, Compare(), ascii::space_type> compare;
    qi::rule<Iterator, Condition(), qi::locals<Condition>, ascii::space_type> condition;
    qi::rule<Iterator, Condition(), qi::locals<Condition>, ascii::space_type> conjunction;
    qi::rule<Iterator, Condition(), ascii::space_type> atomic_condition;
    qi::rule<Iterator, std::string(), ascii::space_type> attribute_name;
    qi::rule<Iterator, Literal(), ascii::space_type>  string_literal;
    qi::rule<Iterator, Literal(), ascii::space_type>  int_literal;
    qi::rule<Iterator, Literal(), ascii::space_type> literal;
    qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> attribute_list;
    qi::rule<Iterator, std::vector<Typed_attribute>(), ascii::space_type> typed_attribute_list;
    qi::rule<Iterator, Attribute_type(), ascii::space_type> type;
    qi::rule<Iterator, std::vector<Assignment>(), ascii::space_type> assignment_list;
    qi::rule<Iterator, Assignment(), ascii::space_type> assignment;

    qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> attr_list_clause;
    qi::rule<Iterator, int(), ascii::space_type> varchar_size_clause;
    qi::rule<Iterator, Condition(), ascii::space_type> condition_clause;

    Compare_op compare_op;
    
    friend Sql_query<Iterator>;
    friend Sql_command_clause<Iterator>;
    friend Sql_command_simple<Iterator>;
    friend Sql_command_create<Iterator>;
    friend Sql_command_update<Iterator>;
    friend Sql_command_delete<Iterator>;
    friend Sql_command_insert<Iterator>;
};

#endif
