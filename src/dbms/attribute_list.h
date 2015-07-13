#ifndef ATTRIBUTE_LIST_H
#define ATTRIBUTE_LIST_H

#include <map>
#include <vector>
#include "attribute.h"
#include "../sqlparser/subgrammar/sql_semantics.h"

using namespace std;

struct Attribute_list {
    typedef map<string, Attribute>::const_iterator const_iterator;
    struct No_such_attribute_exception : exception {
        virtual const char* what() const noexcept { return "No_such_attribute_exception"; }
    };
    struct Incompatible_attribute_name_list_exception : exception {
        virtual const char* what() const noexcept { return "Incompatible_attribute_name_list_exception"; }
    };

    Attribute_list() { }
    Attribute_list(const vector<Typed_attribute>& typed_attrs);

    const Attribute& operator[](const string& name) const;
    const Attribute& operator[](int column) const;
    void add(const string& name, const Attr_type& type, int char_limit=11);
    void add(const Attribute& attr);
    void add(const Typed_attribute& typed_attr); // from sql semantic structure
    void set_primary_keys(const vector<string>& names);
    const vector<string>& get_primary_keys() const { return primary_keys; }
    string generate_key(const vector<string>& row) const;
    void rename(const vector<string>& names);

    Attribute_list project(const vector<string>& names) const;
    Attribute_list operator+(const Attribute_list& other) const;

    const_iterator begin() const { return attributes.begin(); }
    const_iterator end() const { return attributes.end(); }

    int get_column_count() const { return columns; }

private:
    map<string, Attribute> attributes;
    vector<string> primary_keys;
    int columns = 0;
};

#endif // ATTRIBUTE_LIST_H

