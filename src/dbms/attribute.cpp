#include <iostream>
#include "attribute.h"
#include <string>
#include "util.h"

using namespace std;

const string Attribute::type_strings[] = { "varchar", "integer" };
const string Attribute::op_strings[] = { "==", "!=", "<", ">", "<=", ">=" };

Attribute::Attribute(const string& name, const Attr_type& type, int column, int char_limit)
    : name(name), type(type), column(column), char_limit(char_limit) {
    this->type_string = type_to_string(type);   
}

string Attribute::make_value(const string& str) const {
    string ret = str;
    if (is_type_compatible(Attr_type::Var_char) || is_integer(str)) {
        if (str.length() > char_limit) {
            ret.resize(char_limit);
        }
        return ret;
    }
    throw Attribute::Incompatible_type_exception();
}

string Attribute::make_value(int val) const {
    if (is_type_compatible(Attr_type::Integer)) {
        return to_string(val);
    }
    throw Attribute::Incompatible_type_exception();
}

string Attribute::make_value(const Literal& literal) const {
    if (literal.type == Literal::Var_char) {
        return make_value(boost::get<string>(literal.value));
    } else {
        return make_value(boost::get<int>(literal.value));
    }
}

template <typename T>
bool Attribute::comparison_helper(const T& a, const T& b, const Operator& op) const {
    bool ret;
    switch(op) {
    case Operator::Equal:
        ret = a == b;
        break;
    case Operator::Not_equal:
        ret = a != b;
        break;
    case Operator::Less_than:
        ret = a < b;
        break;
    case Operator::Greater_than:
        ret = a > b;
        break;
    case Operator::Less_equal:
        ret = a <= b;
        break;
    case Operator::Greater_equal:
        ret = a >= b;
        break;
    }
    return ret;
}

bool Attribute::compare_values(const string& a, const string& b, const Operator& op) const {
    if (is_type_compatible(Attr_type::Var_char)) {
        return comparison_helper(a, b, op);
    } else { // type is Attr_type::Integer
        return comparison_helper(stoi(a), stoi(b), op);
    }
}

bool Attribute::compare_values(const string& a, int b, const Operator& op) const {
    if (is_type_compatible(Attr_type::Integer)) {
        return comparison_helper(stoi(a), b, op);
    }
    throw Incompatible_type_exception();
}
