#ifndef SQL_SEMANTICS_H
#define SQL_SEMANTICS_H

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_wrapper.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <utility>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace spirit = boost::spirit;
namespace phoenix = boost::phoenix;

struct Literal {
    enum Type { Var_char, Integer };
    typedef boost::variant<std::string, int> Value;

    Type type;
    Value value;
};
BOOST_FUSION_ADAPT_STRUCT(
    Literal,
    (Literal::Type, type)
    (Literal::Value, value)
)
inline std::ostream& operator<<(std::ostream& os, const Literal& literal) {
    return os << "<Literal type=" << (int)literal.type << " value=" << literal.value << " />";
}

struct Compare_op : qi::symbols<char, unsigned> {
    enum Type { EQ, LE, GE, LT, GT, NE };
    Compare_op() {
        add ("==", EQ)
            ("<=", LE)
            (">=", GE)
            ("<", LT)
            (">", GT)
            ("!=", NE)
        ;
    }
};

struct Compare {
    enum Type { EQ, LE, GE, LT, GT, NE };
    typedef boost::variant<std::string, Literal> Operand_type;
    Operand_type left;
    Compare_op::Type op;
    Operand_type right;
};
BOOST_FUSION_ADAPT_STRUCT(
    Compare,
    (Compare::Operand_type, left)
    (Compare_op::Type, op)
    (Compare::Operand_type, right)
)
inline std::ostream& operator<<(std::ostream& os, const Compare& compare) {
    return os << "<Compare left=" << compare.left << " op=" << (int)compare.op << " right=" << compare.right << " />";
}

struct Condition {
    enum Op_type { None, And, Or };
    typedef boost::variant<Compare, boost::recursive_wrapper<Condition>> Node_type;

    Node_type left;
    Op_type op;
    Node_type right;

    Condition() : op(None) {}
};
BOOST_FUSION_ADAPT_STRUCT(
    Condition,
    (Condition::Node_type, left)
    (Condition::Op_type, op)
    (Condition::Node_type, right)
)
inline std::ostream& operator<<(std::ostream& os, const Condition& condition) {
    if (condition.op == Condition::None) {
        return os << condition.left;
    } else {
        return os << "<Condition left=" << condition.left << " op=" << (int)condition.op << " right=" << condition.right << " />";
    }
}

struct Expr {
    typedef boost::variant<std::string, boost::recursive_wrapper<Expr>> Node_type;
    enum Type { None, Select, Project, Rename, Product, Difference, Union, Relation_name };
    Type type;
    Condition condition;
    std::vector<std::string> attribute_list;
    Node_type expr; // left
    Node_type expr2; // right
    
    Expr() : type(None) { }
};
BOOST_FUSION_ADAPT_STRUCT(
    Expr,
    (Expr::Type, type)
    (Condition, condition)
    (std::vector<std::string>, attribute_list)
    (Expr::Node_type, expr)
    (Expr::Node_type, expr2)
)
inline std::ostream& operator<<(std::ostream& os, const Expr& expr) {
    os << "<Expr type=" << (int)expr.type << " condition=" << expr.condition << " attribute_list={ ";
    for (auto str : expr.attribute_list) {
        os << '"' << str << "\" ";
    }
    os << " } expr1=" << expr.expr;
    switch (expr.type) {
        case Expr::Union:
        case Expr::Difference:
        case Expr::Product:
            os << " expr2=" << expr.expr2;
            break;
    }
    os << " />";
    return os;
}

struct Query {
    std::string relation_name;
    Expr expr;
};
BOOST_FUSION_ADAPT_STRUCT(
    Query,
    (std::string, relation_name)
    (Expr, expr)
)
inline std::ostream& operator<<(std::ostream& os, const Query& query) {
    return os << "<Query relation_name=" << query.relation_name << " expr=" << query.expr << " />";
}

struct Attribute_type {
    std::string str;
    int max_length;
    Attribute_type() : max_length(11) { }
};
BOOST_FUSION_ADAPT_STRUCT(
    Attribute_type,
    (std::string, str)
    (int, max_length)
)
inline std::ostream& operator<<(std::ostream& os, const Attribute_type& attr_type) {
    return os << "<Attribute_type str=" << attr_type.str << " max_length=" << attr_type.max_length << " />";
}

struct Assignment {
    std::string attribute_name;
    Literal literal;
};
BOOST_FUSION_ADAPT_STRUCT(
    Assignment,
    (std::string, attribute_name)
    (Literal, literal)
)
inline std::ostream& operator<<(std::ostream& os, const Assignment& assignment) {
    return os << "<Assignment attribute_name=" << assignment.attribute_name << " literal=" << assignment.literal << " />";
}

struct Typed_attribute {
    std::string attribute_name;
    Attribute_type type;
};
BOOST_FUSION_ADAPT_STRUCT(
    Typed_attribute,
    (std::string, attribute_name)
    (Attribute_type, type)
)
inline std::ostream& operator<<(std::ostream& os, const Typed_attribute& typed_attr) {
    return os << "<Typed_attribute type=" << typed_attr.attribute_name << " type=" << typed_attr.type << " />";
}

struct Command {
    enum Cmd_type { Open, Close, Write, Exit, Show, Create, Update, Insert, Delete };
    Cmd_type type;
    std::string relation_name;
    std::vector<Typed_attribute> column_names;
    std::vector<std::string> primary_keys;
    std::vector<Assignment> assignment_list;
    Condition condition;
    std::vector<Literal> row;
    Expr expr;
};
BOOST_FUSION_ADAPT_STRUCT(
    Command,
    (Command::Cmd_type, type) // 0
    (std::string, relation_name) // 1
    (std::vector<Typed_attribute>, column_names) // 2
    (std::vector<std::string>, primary_keys) // 3
    (std::vector<Assignment>, assignment_list) // 4
    (Condition, condition) // 5
    (std::vector<Literal>, row) // 6
    (Expr, expr) // 7
)
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& list) {
    os << "{ ";
    for (const T& item : list) {
        os << item << ' ';
    }
    return os << " }";
}
inline std::ostream& operator<<(std::ostream& os, const Command& cmd) {
    os << "<Command type=" << (int)cmd.type;
    if (cmd.type == Command::Exit) {
        return os << " />";
    } else if (cmd.type == Command::Show || (cmd.type == Command::Insert && cmd.row.empty())) {
        os << " expr=" << cmd.expr;
    } else {
        os << " relation_name=" << cmd.relation_name;
        switch (cmd.type) {
            case Command::Create:
                os << " column_names=" << cmd.column_names << " primary_keys=" << cmd.primary_keys;
                break;
            case Command::Update:
                os << " assignment_list=" << cmd.assignment_list << " condition=" << cmd.condition;
                break;
            case Command::Insert: 
                if (!cmd.row.empty()) {
                    os << " row=" << cmd.row;
                }
                break;
            case Command::Delete:
                os << " condition=" << cmd.condition;
                break;
        }
    }
    return os << " />";
}

struct Program {
    enum Stmt_type { Sql_query, Sql_command };
    Stmt_type type;
    Query query;
    Command command;
};
BOOST_FUSION_ADAPT_STRUCT(
    Program,
    (Program::Stmt_type, type)
    (Query, query)
    (Command, command)
)
inline std::ostream& operator<<(std::ostream& os, const Program& program) {
    os << "<Program type=" << program.type;
    switch (program.type) {
        case Program::Sql_query:
            os << " query=" << program.query;
            break;
        case Program::Sql_command:
            os << " command=" << program.command;
            break;
    }
    return os << " />";
}

struct StringLiteral {
    template <typename T1, typename T2 = void>
    struct result { typedef void type; };

    void operator()(Literal& literal, char ch) const {
        boost::get<std::string>(literal.value).push_back(ch);
    }
};
static const phoenix::function<StringLiteral> string_literal_builder;

#endif
