#ifndef SQL_QUERY_H
#define SQL_QUERY_H

#include "sql_basic.h"

template <typename Iterator> struct Sql_parser;

template <typename Iterator>
struct Sql_query: qi::grammar<Iterator, Query(), ascii::space_type> {
    Sql_query() : Sql_query::base_type(query) {
        selection = 
            lit("select")              [at_c<0>(_val) = Expr::Select]
            >> basic.condition_clause   [at_c<1>(_val) = _1]
            >> atomic_expr              [at_c<3>(_val) = _1]
            ;
        projection =
            lit("project")           [at_c<0>(_val) = Expr::Project]
            >> basic.attr_list_clause [at_c<2>(_val) = _1] 
            >> atomic_expr            [at_c<3>(_val) = _1]
            ;
        renaming =
            lit("rename")            [at_c<0>(_val) = Expr::Rename]
            >> basic.attr_list_clause [at_c<2>(_val) = _1]
            >> atomic_expr            [at_c<3>(_val) = _1]
            ;
        set_operator =
            lit("+")    [_val = Expr::Union]
            | lit("-")  [_val = Expr::Difference]
            | lit("*")  [_val = Expr::Product]
            ;
        set_operation = 
            atomic_expr    [at_c<3>(_val) = _1]
            >> set_operator [at_c<0>(_val) = _1]
            >> atomic_expr  [at_c<4>(_val) = _1]
            ;
        relation_name =
            basic.relation_name [at_c<0>(_val) = Expr::Relation_name, at_c<3>(_val) = _1]
            ;
        expr %= 
            selection
            | projection
            | renaming 
            | set_operation
            | atomic_expr
            ;
        atomic_expr %= 
            expr_clause 
            | relation_name
            ;
        query = basic.relation_name >> "<-" >> expr >> ';';

        expr_clause %=
            '('
            >> expr
            >> ')'
            ;
    }
private:
    qi::rule<Iterator, Query(), ascii::space_type> query;
    qi::rule<Iterator, Expr(), ascii::space_type> expr;
    qi::rule<Iterator, Expr(), ascii::space_type> atomic_expr;
    qi::rule<Iterator, Expr(), ascii::space_type> relation_name;
    qi::rule<Iterator, Expr(), ascii::space_type> selection;
    qi::rule<Iterator, Expr(), ascii::space_type> projection;
    qi::rule<Iterator, Expr(), ascii::space_type> renaming;
    qi::rule<Iterator, Expr(), ascii::space_type> set_operation;
    qi::rule<Iterator, Expr::Type(), ascii::space_type> set_operator;

    qi::rule<Iterator, Expr(), ascii::space_type> expr_clause;

    Sql_basic<Iterator> basic;

    friend Sql_command_clause<Iterator>;
    friend Sql_command_create<Iterator>;
    friend Sql_command_update<Iterator>;
    friend Sql_command_delete<Iterator>;
    friend Sql_command_insert<Iterator>;
    friend Sql_command_simple<Iterator>;
    friend Sql_parser<Iterator>;
};
typedef std::string::const_iterator str_const_it;
std::pair<bool, Query> parse_query(str_const_it begin, str_const_it end);
#endif
