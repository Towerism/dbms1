#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>
#include <vector>
#include <tuple>
#include <exception>
#include <cmath>

#include "../sqlparser/subgrammar/sql_semantics.h"

using namespace std;

struct Attribute {
    enum class Type : int {
        Var_char, Integer
    };

    enum class Operator : int {
        Equal, Not_equal, Less_than, Greater_than, Less_equal, Greater_equal
    };

    struct Incompatible_type_exception : exception {
        virtual const char* what() const noexcept { return "Incompatible_type_exception"; }
    };

    Attribute() : name(""), type_string(""), column(0), char_limit(11) { }
    Attribute(const string& name, const Type& type, int column, int char_limit=11);
    
    string make_value(const string& str) const;
    string make_value(int val) const;
    string make_value(const Literal& literal) const;

    bool compare_values(const string& a, const string& b, const Operator& op) const;
    bool compare_values(const string& a, int b, const Operator& op) const;

    const string& get_name() const { return name; }
    const Type& get_type() const { return type; }
    const string& get_type_string() const { return type_string; }
    int get_column() const { return column; }
    int get_char_limit() const { return char_limit; }
    int get_column_width() const { return max(char_limit, (int)name.length()); }
    void set_name(const string& str) { name = str; }
    void set_column(int val) { column = val; }

private:
    string name;
    Type type;
    string type_string;
    int column;
    int char_limit;

    static const string type_strings[];
    const string& type_to_string(const Type& val) { return type_strings[(int)val]; }

    static const string op_strings[];
    static const string& op_to_string(const Operator& op) { return op_strings[(int)op]; }

    bool is_type_compatible(const Type& type) const { return this->type == type; }
    
    template <typename T>
    bool comparison_helper(const T& a, const T& b, const Operator& op) const;
};

typedef Attribute::Type Attr_type;
typedef Attribute::Operator Attr_operator;
typedef tuple<string, string, Attr_operator> Attr_constraint;
typedef vector<Attr_constraint> Attr_constraints;

#endif // ATTRIBUTE_H
