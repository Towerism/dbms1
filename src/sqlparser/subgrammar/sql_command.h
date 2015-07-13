#ifndef SQL_COMMAND_H
#define SQL_COMMAND_H

#include "sql_query.h"

template <typename Iterator>
struct Sql_command_clause : qi::grammar<Iterator, ascii::space_type> {
    Sql_command_clause() : Sql_command_clause::base_type(qi::eps) {
        pkey %= 
            lit("PRIMARY KEY") 
            >> basic.attr_list_clause
            ;
        typed_alist_clause %=
            '('
            >> basic.typed_attribute_list
            >> ')'
            ;
        set %=
            "SET"
            >> basic.assignment_list
            ;
        where %=
            "WHERE" 
            >> basic.condition
            ;
        values_from_exp %=
            "VALUES FROM RELATION" 
            >> query.expr
            ;
        values_from %=
            lit("VALUES FROM") 
            >> '(' 
            >> (basic.literal % ',')
            >> ')'
            ;
    }
private:
    qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> pkey;
    qi::rule<Iterator, std::vector<Assignment>(), ascii::space_type> set;
    qi::rule<Iterator, std::vector<Typed_attribute>(), ascii::space_type> typed_alist_clause;
    qi::rule<Iterator, Condition(), ascii::space_type> where;
    qi::rule<Iterator, Expr(), ascii::space_type> values_from_exp;
    qi::rule<Iterator, std::vector<Literal>(), ascii::space_type> values_from;

    Sql_basic<Iterator> basic;
    Sql_query<Iterator> query;

    friend Sql_command_create<Iterator>;
    friend Sql_command_update<Iterator>;
    friend Sql_command_delete<Iterator>;
    friend Sql_command_insert<Iterator>;
};

template <typename Iterator>
struct Sql_command_simple : qi::grammar<Iterator, Command(), ascii::space_type> {
    Sql_command_simple() : Sql_command_simple::base_type(start) {
        start %= (open_cmd | close_cmd | write_cmd | exit_cmd | show_cmd) >> ';';

        open_cmd = 
            lit("OPEN")           [at_c<0>(_val) = Command::Open]
            >> basic.relation_name [at_c<1>(_val) = _1]
            ;
        close_cmd = 
            lit("CLOSE")           [at_c<0>(_val) = Command::Close]
            >> basic.relation_name  [at_c<1>(_val) = _1]
            ;
        write_cmd = 
            lit("WRITE")           [at_c<0>(_val) = Command::Write]
            >> basic.relation_name  [at_c<1>(_val) = _1]
            ;
        exit_cmd = lit("EXIT")  [at_c<0>(_val) = Command::Exit];
        show_cmd = 
            lit("SHOW")         [at_c<0>(_val) = Command::Show]
            >> query.atomic_expr [at_c<7>(_val) = _1]
            ;
    }
private:
    qi::rule<Iterator, Command(), ascii::space_type> start;
    qi::rule<Iterator, Command(), ascii::space_type> open_cmd;
    qi::rule<Iterator, Command(), ascii::space_type> close_cmd;
    qi::rule<Iterator, Command(), ascii::space_type> write_cmd;
    qi::rule<Iterator, Command(), ascii::space_type> exit_cmd;
    qi::rule<Iterator, Command(), ascii::space_type> show_cmd;

    Sql_basic<Iterator> basic;
    Sql_query<Iterator> query;
};

template <typename Iterator>
struct Sql_command_create: qi::grammar<Iterator, Command(), ascii::space_type> {
    Sql_command_create() : Sql_command_create::base_type(start) {
        start %= create_cmd >> ';';
        
        create_cmd = 
            lit("CREATE TABLE")          [at_c<0>(_val) = Command::Create]
            >> basic.relation_name        [at_c<1>(_val) = _1]
            >> clause.typed_alist_clause         [at_c<2>(_val) = _1]
            >> clause.pkey                [at_c<3>(_val) = _1]
            ;
    }
private:
    qi::rule<Iterator, Command(), ascii::space_type> start;
    qi::rule<Iterator, Command(), ascii::space_type> create_cmd;

    Sql_basic<Iterator> basic;
    Sql_command_clause<Iterator> clause;
};

template <typename Iterator>
struct Sql_command_update: qi::grammar<Iterator, Command(), ascii::space_type> {
    Sql_command_update() : Sql_command_update::base_type(start) {
        start %= update_cmd >> ';';
        
        update_cmd = 
            lit("UPDATE")           [at_c<0>(_val) = Command::Update]
            > basic.relation_name   [at_c<1>(_val) = _1]
            > clause.set            [at_c<4>(_val) = _1]
            > clause.where          [at_c<5>(_val) = _1]
            ;
        qi::on_error<qi::fail>
        (
         update_cmd,
         std::cout
         << phoenix::val("Error! Expecting ")
         << qi::_4
         << phoenix::val(" here: \"")
         << phoenix::construct<std::string>(qi::_3, qi::_2)
         << boost::phoenix::val("\"")
         << std::endl
        );
    }
private:
    qi::rule<Iterator, Command(), ascii::space_type> start;
    qi::rule<Iterator, Command(), ascii::space_type> update_cmd;

    Sql_basic<Iterator> basic;
    Sql_command_clause<Iterator> clause;
};

template <typename Iterator>
struct Sql_command_delete: qi::grammar<Iterator, Command(), ascii::space_type> {
    Sql_command_delete() : Sql_command_delete::base_type(start) {
        start %= delete_cmd >> ';';

        delete_cmd = 
            lit("DELETE FROM" )       [at_c<0>(_val) = Command::Delete]
            >> basic.relation_name     [at_c<1>(_val) = _1]
            >> clause.where            [at_c<5>(_val) = _1]
            ;
    }
private:
    qi::rule<Iterator, Command(), ascii::space_type> start;
    qi::rule<Iterator, Command(), ascii::space_type> delete_cmd;

    Sql_basic<Iterator> basic;
    Sql_command_clause<Iterator> clause;
};

template <typename Iterator>
struct Sql_command_insert: qi::grammar<Iterator, Command(), ascii::space_type> {
    Sql_command_insert() : Sql_command_insert::base_type(start) {
        start %= insert_cmd >> ';';
        
        insert_cmd = 
            lit("INSERT INTO")          [at_c<0>(_val) = Command::Insert]
            >> basic.relation_name       [at_c<1>(_val) = _1] 
            >> (clause.values_from_exp   [at_c<7>(_val) = _1]
                | clause.values_from    [at_c<6>(_val) = _1]
                )
            ;
    }
private:
    qi::rule<Iterator, Command(), ascii::space_type> start;
    qi::rule<Iterator, Command(), ascii::space_type> insert_cmd;

    Sql_basic<Iterator> basic;
    Sql_command_clause<Iterator> clause;
};

std::pair<bool, Command> parse_command(str_const_it begin, str_const_it end);
std::pair<bool, Command> parse_create_command(str_const_it begin, str_const_it end);
std::pair<bool, Command> parse_update_command(str_const_it begin, str_const_it end);
std::pair<bool, Command> parse_delete_command(str_const_it begin, str_const_it end);
std::pair<bool, Command> parse_insert_command(str_const_it begin, str_const_it end);
std::pair<bool, Command> parse_simple_command(str_const_it begin, str_const_it end);
std::pair<bool, Command> parse_complex_command(str_const_it begin, str_const_it end);
#endif
