#include <iostream>
#include <limits>
#include <fstream>
#include <exception>
#include "database_manager.h"
#include <vector>

using namespace std;

int main() {
    Database_manager dbms;
    try {
        dbms.start();
    } catch (const exception& e) {
        cerr << e.what();
    }
}
