#include "database_manager.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <boost/variant.hpp>

using namespace std;

void Database_manager::start() {
    running = true;
    while (running) {
        string sql_stmt;
        cout << "\n:: DBMS> ";
        getline(cin,sql_stmt);
        execute(sql_stmt);
    }
}

void Database_manager::exit() { 
    running = false;
}

// executes a sql query or command
// return type should be a Relation
void Database_manager::execute(const string& str) { 
    auto result_program = parse_program(str.begin(), str.end());
    if (!result_program.first) {
        cout << "Bad syntax\n";
        return;
    }
    interpret_program(result_program.second);
}

void Database_manager::interpret_program(const Program& program) {
    switch (program.type) {
        case Program::Sql_query:
            interpret_query(program.query);
            break;
        case Program::Sql_command:
            interpret_command(program.command);
            break;
    }
}

void Database_manager::interpret_query(const Query& query) {
        Relation* rel = relation(query.relation_name);
        Relation expr = interpret_expr(query.expr);
        if (rel != nullptr) {
            close(query.relation_name);
        }    
        create(query.relation_name, expr);
}

void Database_manager::interpret_command(const Command& cmd) {
    Relation* rel = nullptr;
    switch (cmd.type) {
        case Command::Open:
            rel = open(cmd.relation_name);
            break;
        case Command::Close:
            close(cmd.relation_name);
            break;
        case Command::Write:
            write(cmd.relation_name);
            break;
        case Command::Exit:
            exit();
            break;
        case Command::Show:
            {
                Relation r = interpret_expr(cmd.expr);
                r.print();
            }
            break;
        case Command::Create:
            {
                Attribute_list attr_list(cmd.column_names);
                attr_list.set_primary_keys(cmd.primary_keys);
                rel = create(cmd.relation_name, attr_list);
                break;
            }
        case Command::Update:
            rel = relation(cmd.relation_name);
            if (rel != nullptr) {
                vector<pair<string, string>> assignments;
                rel->update_if(cmd.assignment_list, cmd.condition);
            }
            break;
        case Command::Insert:
            rel = relation(cmd.relation_name);
            if (cmd.expr.type != Expr::None) {
                rel->add(interpret_expr(cmd.expr));
            } else {
                rel->add(cmd.row);
            }
            break;
        case Command::Delete:
            rel = relation(cmd.relation_name);
            rel->delete_if(cmd.condition);
            break;
    }
}

Relation Database_manager::interpret_expr(const Expr& expr) {
    Relation rel; // left
    Relation rel2; // right
    switch (expr.type) {
        case Expr::Select:
            {
                rel = interpret_expr(boost::get<Expr>(expr.expr));
                rel = rel.select(expr.condition);
            }
            break;
        case Expr::Relation_name:
            {
                string name = boost::get<string>(expr.expr);
                Relation* r = relation(name);
                if (r != nullptr) {
                    rel = *r; 
                } else {
                    throw No_such_relation_exception();
                }
            }
            break;
        case Expr::Project:
            {
                auto attr_list = expr.attribute_list;
                rel = interpret_expr(boost::get<Expr>(expr.expr));
                rel = rel.project(attr_list);
            }
            break;
        case Expr::Rename:
            {
                auto attr_list = expr.attribute_list;
                rel = interpret_expr(boost::get<Expr>(expr.expr));
                rel = rel.rename(attr_list);
            }
            break;
        case Expr::Product:
            {
                rel = interpret_expr(boost::get<Expr>(expr.expr));
                rel2 = interpret_expr(boost::get<Expr>(expr.expr2));
                rel = rel * rel2;
            }
            break;
        case Expr::Difference:
            {
                rel = interpret_expr(boost::get<Expr>(expr.expr));
                rel2 = interpret_expr(boost::get<Expr>(expr.expr2));
                rel = rel - rel2;
            }
            break;
        case Expr::Union:
            {
                rel = interpret_expr(boost::get<Expr>(expr.expr));
                rel2 = interpret_expr(boost::get<Expr>(expr.expr2));
                rel = rel + rel2;
            }
            break;
    }
    return rel;
}

Relation* Database_manager::relation(const string& name) {
    if (relation_map.find(name) != relation_map.end()) {
        return relation_map[name];
    }
    return nullptr;
}

//opens a filestream to create a relation
Relation* Database_manager::open(const string& name) {
    if (relation(name) != nullptr) {
        cout << "Relation is already open\n";
        return nullptr;
    }
    fstream file(name + ".db", ios_base::in);
    string label, attr_name, temp_str;
    int attr_type = 0, attr_char_limit = 0;
    vector<string> row;
    vector<string> pkeys;
    Attribute_list attributes;
    file >> label >> label; // Keys 
    int size = 0;
    file >> size;
    file.ignore(numeric_limits<int>::max(), '\n'); // throw out any trailing characters
    for (int i = 0; i < size; ++i) {
        getline(file, temp_str);
        pkeys.push_back(temp_str);
    }
    file >> label;
    while (label == "--ATTRIBUTE") {
        file.ignore(numeric_limits<int>::max(), '\n');
        getline(file, attr_name); 
        file >> attr_type;
        file >> attr_char_limit;
        attributes.add(attr_name, (Attr_type)attr_type, attr_char_limit);
        file >> label;
    }
    attributes.set_primary_keys(pkeys);
    Relation* relation = new Relation(name, attributes);
    file >> label >> label; // Rows > Items
    while (label == "--ITEM") {
        row.clear();
        file.ignore(numeric_limits<int>::max(), '\n');
        for (int i = 0; i < attributes.get_column_count(); ++i) {
            string item;
            while (true) {
                getline(file, temp_str);
                if (temp_str == "--ITEM") {
                    break;
                } else if (temp_str == "--ROW") {
                    file >> label;
                    break;
                } else if (temp_str == "--END") { // --END
                    label = "--END";
                    break;
                }
                item += temp_str;
            }
            row.push_back(item);
        }
        relation->add(row);
    }
    relation_map[name] = relation;
    return relation;
}

Relation* Database_manager::create(const string& name, const Attribute_list& attributes) {
    map<string, Relation*>::iterator it = relation_map.find(name);
    Relation* relation;
    if (it == relation_map.end()) {
        relation = new Relation(name, attributes);
        relation_map[name] = relation;
    } else {
        cerr << "Relation already exists with that name: " << name << endl;
        return nullptr;
    }
    return relation;
}

Relation* Database_manager::create(const string& name, const Relation& relation) {
    Relation* rel = create(name, relation.get_attributes());
    rel->add(relation);
    return rel;
}

//closes the filestream
void Database_manager::close(const string& name){
    map<string, Relation*>::iterator it = relation_map.find(name);
    if(it != relation_map.end()) {
        delete it->second;
        relation_map.erase(it);
    } else {
        cerr << "Relation hasn't been opened yet: " << name << endl;
    }
}

//writes the relation from memory to the file in ASCII text
void Database_manager::write(const string& name){
    // write attributes
    fstream file(name + ".db", ios_base::out | ios_base::trunc);
    map<string, Relation*>::iterator it = relation_map.find(name);
    if (it == relation_map.end()) {
        cerr << "Relation does not exist";
        return;
    }
    const auto& relation = *(it->second);
    const auto& attributes = relation.get_attributes();
    file << "--ATTRIBUTES\n";
    file << "--KEYS\n";
    file << attributes.get_primary_keys().size() << endl;
    for (const auto& key : attributes.get_primary_keys()) {
        file << key << endl;
    }
    for (int i = 0; i < attributes.get_column_count(); ++i) {
        const auto& attr = attributes[i];
        file << "--ATTRIBUTE\n";
        file << attr.get_name() << endl;
        file << (int)(attr.get_type()) << endl;
        file << attr.get_char_limit() << endl;
    }
    const auto& rows = relation.get_rows();
    file << "--ROWS\n";
    for (const auto& elem : rows) {
        const auto& row = elem.second;
        file << "--ROW\n";
        for (const auto& item : row) {
            file << "--ITEM" << endl;
            file << item << endl;
        }
    }
    file << "--END\n";
}

