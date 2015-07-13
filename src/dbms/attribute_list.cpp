#include <algorithm>
#include <iostream>
#include "attribute_list.h"

Attribute_list::Attribute_list(const vector<Typed_attribute>& typed_attrs) {
    for (const auto& typed_attr : typed_attrs) {
        add(typed_attr);
    }
}

const Attribute& Attribute_list::operator[](const string& name) const {
    return attributes.at(name);
}

const Attribute& Attribute_list::operator[](int column) const {
    for (const auto& item : attributes) {
        if (item.second.get_column() == column) {
            return item.second;
        }
    }
}

void Attribute_list::add(const string& name, const Attr_type& type, int char_limit) {
    Attribute attr(name, type, columns++, char_limit);
    attributes[name] = attr;
}

void Attribute_list::add(const Attribute& attr) {
    add(attr.get_name(), attr.get_type(), attr.get_char_limit());
}

void Attribute_list::add(const Typed_attribute& typed_attr) {
    Attr_type type = typed_attr.type.str == "VARCHAR" ? Attr_type::Var_char : Attr_type::Integer;
    add(typed_attr.attribute_name, type, type == Attr_type::Integer ? 11 : typed_attr.type.max_length);
}

void Attribute_list::set_primary_keys(const vector<string>& names) {
    primary_keys.clear();
    for (const auto& name : names) {
        const auto& it = attributes.find(name);
        if (it != attributes.end()) {
            primary_keys.push_back(name);
        } else {
            throw No_such_attribute_exception();
        }
    }
}

string Attribute_list::generate_key(const vector<string>& row) const {
    string key;
    for (const auto& pkey : primary_keys) {
        int column = attributes.at(pkey).get_column();
        key += row[column];
    }
    return key;
}

void Attribute_list::rename(const vector<string>& names) {
    if (names.size() != attributes.size()) {
        throw Incompatible_attribute_name_list_exception();
    }
    for (int i = 0; i < names.size(); ++i) {
        auto attr_it = find_if(attributes.begin(), attributes.end(), [i](const pair<string, Attribute>& key_attr) { return key_attr.second.get_column() == i; });
        Attribute attr = attr_it->second;
        attributes.erase(attr_it);
        auto key_it = find(primary_keys.begin(), primary_keys.end(), attr.get_name());
        attr.set_name(names[i]);
        attributes[names[i]] = attr;
        if (key_it != primary_keys.end()) {
            *key_it = names[i];
        }
    }
}

Attribute_list Attribute_list::project(const vector<string>& names) const {
    Attribute_list list;
    vector<string> pkeys;
    for (const auto& name : names) {
        const_iterator it = find_if(attributes.begin(), attributes.end(), 
            [name](const pair<string, Attribute>& elem) -> bool { return elem.second.get_name() == name; } );
        if (it == attributes.end()) {
            throw No_such_attribute_exception();
        }
        list.add(it->second);
        pkeys.push_back(it->second.get_name());
    }
    list.set_primary_keys(pkeys);
    return list;
}

Attribute_list Attribute_list::operator+(const Attribute_list& other) const {
    Attribute_list list(*this);
    for (int i = 0; i < other.columns; ++i) {
        Attribute other_attr = other[i];
        other_attr.set_column(i + list.columns);
        list.attributes[other_attr.get_name()] = other_attr;
    }
    list.columns += other.columns;
    list.primary_keys.insert(list.primary_keys.end(), other.primary_keys.begin(), other.primary_keys.end());
    return list;
}
