#ifndef RELATION_H
#define RELATION_H

#include <string>
#include <map>
#include "attribute_list.h"
#include "../sqlparser/subgrammar/sql_semantics.h"
using namespace std;

struct Relation {
    struct Incompatible_relations_exception : exception {
        virtual const char* what() const noexcept { return "Incompatible_relations_exception"; }
    };
    Relation() { }
    Relation(const string& name, const Attribute_list& attributes)
        : name(name), attributes(attributes) { } 

    void add(const vector<string>& row);
    void add(const vector<Literal>& row);
    void add(const Relation& rel);
    void print() const;

    const vector<string>& get_row(const vector<string>& keys) const;

    Relation select(Condition condition) const;
    Relation project(const vector<string>& attr_names) const;
    Relation rename(const vector<string>& attr_names) const;
    
    void update_if(const vector<Assignment>& assignments, const Condition& condition);
    void delete_if(const Condition& condition);

    Relation operator+(const Relation& other) const; // union
    Relation operator-(const Relation& other) const; // difference
    Relation operator*(const Relation& other) const; // product
    // get/set
    const string& get_name() const { return name; }
    void set_name(const string& str) { name = str; }
    const Attribute_list& get_attributes() const { return attributes; }
    const map<string, vector<string>>& get_rows() const { return rows; }
private:
    string name;
    Attribute_list attributes;
    map<string, vector<string>> rows;

    void select_helper(Relation& relation, Condition) const;
    bool is_union_compatible_with(const Relation& other) const;
    bool is_disjoint_from(const Relation& other) const;

    void print_hline() const;
    void print_attributes() const;
    void print_rows() const;

    bool traverse_conditional(const vector<string>& row, Condition condition) const;
    bool eval_comparison(const vector<string>& row, Compare compare) const;

};

#endif // RELATION_H
