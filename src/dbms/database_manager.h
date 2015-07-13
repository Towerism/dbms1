#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "relation.h"
#include "attribute_list.h"
#include "../sqlparser/sql_parser.h"

using namespace std;

struct Database_manager {
    struct No_such_relation_exception : exception {
        virtual const char* what() const noexcept { return "No_such_relation_exception"; }
    };
    Database_manager() {};
    
    Relation* open(const string& name);
    Relation* create(const string& name, const Attribute_list& attributes);
    Relation* create(const string& name, const Relation& relation);
    void close(const string& name);
    void write(const string& name);
    void execute(const string& str);
    void exit();
    void start();
    const Relation& get_relation(const string& name) { return *relation(name); }
private:
    map<string, Relation*> relation_map;

    bool running;

    Relation* relation(const string& name);
    void interpret_program(const Program& program);
    void interpret_query(const Query& query);
    void interpret_command(const Command& command);
    Relation interpret_expr(const Expr& expr);
};

#endif // DATABASE_H
