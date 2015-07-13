#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include "relation.h"

using namespace std;

void Relation::add(const vector<string>& row) {
    if (row.size() != attributes.get_column_count()) {
        cout << "Incompatible row size\n";
        return;
    }
    vector<string> temp_row;
    string value;
    for  (int i = 0; i < row.size(); ++i) {
        value = attributes[i].make_value(row[i]);
        temp_row.push_back(move(value));
    }
    string key = attributes.generate_key(temp_row);
    rows[key] = temp_row;
}

void Relation::add(const vector<Literal>& row) {
    vector<string> row_strings;
    for (const auto& literal : row) {
        if (literal.type == Literal::Var_char) {
            row_strings.push_back(boost::get<string>(literal.value));
        } else {
            row_strings.push_back(to_string(boost::get<int>(literal.value)));
        }
    }
    add(row_strings);
}

void Relation::add(const Relation& rel) {
    for (auto elem : rel.rows) {
        auto row = elem.second;
        vector<string> temp_row;
        for (const string& str : row) {
            temp_row.push_back(str);
        }
        string key = rel.attributes.generate_key(temp_row);
        rows[key] = temp_row;
    }
}

const vector<string>& Relation::get_row(const vector<string>& keys) const {
    string key;
    for (const string& str : keys) {
        key += str;
    }
    return rows.at(key);
}

void Relation::print_attributes() const {
    for (int i = 0; i < attributes.get_column_count(); ++i) {
        const Attribute& attr = attributes[i];
        int padding = attr.get_column_width();
        cout << left << setw(padding) << attr.get_name();
        if (i != attributes.get_column_count() - 1) {
            cout << " | ";
        }
    }
    cout << endl;
}

void Relation::print_hline() const {
    for (int i = 0; i < attributes.get_column_count(); ++i) {
        for (int j = 0; j < attributes[i].get_column_width(); ++j) {
        cout << '-';
        }
        cout << "---";
    }
    cout << endl;
}

void Relation::print_rows() const {
    for (const auto& elem : rows) {
        const auto& row = elem.second;
        for (int i = 0; i < row.size(); ++i) {
            int padding = attributes[i].get_column_width();
            cout << left << setw(padding) << row[i];
            if (i != row.size() - 1) {
                cout << " | ";
            }
        }
        cout << endl;
    }
}    

void Relation::print() const {
    cout << "Table name: " << name << endl;
    print_hline();
    print_attributes();
    print_hline();
    print_rows();
    print_hline();
}

Relation Relation::select(Condition condition) const {
    Relation relation("", attributes);
    select_helper(relation, condition);
    return relation;
}

void Relation::select_helper(Relation& relation, Condition condition) const {
    for (const auto& elem : rows) {
        const auto& row = elem.second;
        if (traverse_conditional(row, condition)) {
            relation.add(row);
        }
    }
}

bool Relation::traverse_conditional(const vector<string>& row, Condition condition) const {
    if (condition.op == Condition::None) {
        return eval_comparison(row, boost::get<Compare>(condition.left));
    }
    bool result_left = traverse_conditional(row, boost::get<Condition>(condition.left));
    if (condition.op == Condition::And) {
        return result_left && traverse_conditional(row, boost::get<Condition>(condition.right));
    }
    else {
        return result_left || traverse_conditional(row, boost::get<Condition>(condition.right));
    }
}

bool Relation::eval_comparison(const vector<string>& row, Compare compare) const {
    bool is_attr_name_on_left = compare.left.which() == 0;
    bool attribute_comparison = false;
    string attr_name = is_attr_name_on_left ? get<string>(compare.left) : get<string>(compare.right); //left
    string attr_name2; // right
    const Attribute& attr = attributes[attr_name];
    string val = row[attr.get_column()]; // left
    string val2; // right
    Attr_operator op;
    if (compare.left.which() == compare.right.which()) {
        attribute_comparison = true;
        attr_name2 = get<string>(compare.right);
        val2 = row[attributes[attr_name2].get_column()];
    }
    if (!attribute_comparison) {
        if (is_attr_name_on_left) {
            if (get<Literal>(compare.right).value.which() == 0) {
                val2 = get<string>(get<Literal>(compare.right).value);
            } else {
                val2 = to_string(get<int>(get<Literal>(compare.right).value));
            }
        } else {
            if (get<Literal>(compare.left).value.which() == 0) {
                val2 = get<string>(get<Literal>(compare.left).value);
            } else {
                val2 = to_string(get<int>(get<Literal>(compare.left).value));
            }
        }
    }
    
    switch (compare.op) {
        case Compare_op::EQ:
            op = Attr_operator::Equal;
            break;
        case Compare_op::LE:
            op = is_attr_name_on_left ? Attr_operator::Less_equal : Attr_operator::Greater_equal;
            break;
        case Compare_op::GE:
            op = is_attr_name_on_left ? Attr_operator::Greater_equal : Attr_operator::Less_equal;
            break;
        case Compare_op::LT:
            op = is_attr_name_on_left ? Attr_operator::Less_than : Attr_operator::Greater_than;
            break;
        case Compare_op::GT:
            op = is_attr_name_on_left ? Attr_operator::Greater_than : Attr_operator::Less_than;
            break;
        case Compare_op::NE:
            op = Attr_operator::Not_equal ;
            break;
    }
    return attr.compare_values(val, val2, op);
}

Relation Relation::project(const vector<string>& attr_names) const {
    Relation relation("", attributes.project(attr_names));
    for (const auto& elem : rows) {
        const auto& row = elem.second;
        vector<string> temp_row;
        for (int i = 0; i < row.size(); ++i) {
            string attr_name = attributes[i].get_name();
            if (any_of(attr_names.begin(), attr_names.end(), 
                [attr_name](const string& name) -> bool { return name == attr_name; } )) {
                temp_row.push_back(row[i]);
            }
        }
        relation.add(temp_row);
    }
    return relation;
}

Relation Relation::rename(const vector<string>& attr_names) const {
    Relation relation(*this); 
    relation.attributes.rename(attr_names);
    return relation;
}

void Relation::update_if(const vector<Assignment>& assignments, const Condition& condition) {
    Relation temp_relation = select(condition);
    for (auto& elem : temp_relation.rows) {
        auto& row = elem.second;
        rows.erase(elem.first);
        for (auto assignment : assignments) {
            string str = attributes[assignment.attribute_name].make_value(assignment.literal);
            int column = attributes[assignment.attribute_name].get_column();
            row[column] = str;
        }
        add(row);
    }
}

void Relation::delete_if(const Condition& condition) {
    Relation temp_relation = select(condition);
    for (const auto& elem : temp_relation.rows) {
        rows.erase(elem.first);
    }
}

Relation Relation::operator+(const Relation& other) const {
    if (!is_union_compatible_with(other)) {
        throw Incompatible_relations_exception();
    }
    Relation relation(*this);
    for (const auto& elem : other.rows) {
        const auto& row = elem.second;
        relation.add(row);
    }
    return relation;
}

Relation Relation::operator-(const Relation& other) const {
    if (!is_union_compatible_with(other)) {
        throw Incompatible_relations_exception();
    }
    Relation relation(*this);
    for (const auto& elem : other.rows) {
        relation.rows.erase(elem.first);
    }
    return relation;
}

Relation Relation::operator*(const Relation& other) const {
    if (!is_disjoint_from(other)) {
        throw Incompatible_relations_exception();
    }
    Relation relation("", attributes + other.attributes);
    for (const auto& elem_i : rows) {
        for (const auto& elem_j : other.rows) {
            auto row_i = elem_i.second;
            const auto& row_j = elem_j.second;
            row_i.insert(row_i.end(), row_j.begin(), row_j.end());
            relation.add(row_i);
        }
    }
    return relation;
}

bool Relation::is_union_compatible_with(const Relation& other) const {
    if (attributes.get_column_count() == other.attributes.get_column_count()) { 
        for (int i = 0; i < attributes.get_column_count(); ++i) {
            if (attributes[i].get_type() != other.attributes[i].get_type()) {
                return false;
            }
        }
    } else {
        return false;
    }
    return true;
}

bool Relation::is_disjoint_from(const Relation& other) const {
    for (const auto& elem : attributes) {
        const string& attr_name = elem.second.get_name();
        if (any_of(other.attributes.begin(), other.attributes.end(),
            [attr_name](const pair<string, Attribute>& elem) -> bool { return elem.second.get_name() == attr_name; } )) {
            return false;
        }
    }
    return true;
}
