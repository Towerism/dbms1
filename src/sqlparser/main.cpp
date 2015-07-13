#include "sql_parser.h"

int main() {
    std::cout << "\t\tSimplified SQL Parser\n\n";
    std::cout << "Type an simplified SQL query or command ...use [Ctrl-C] to quit\n\n";

    typedef std::string::const_iterator iterator_type;

    std::string str;
    while (std::cout << "> ", std::getline(std::cin, str)) {
        iterator_type iter = str.begin();
        iterator_type end = str.end();
        auto result_program = parse_program(iter, end);

        if (result_program.first) {
            std::cout << result_program.second << std::endl;
            std::cout << "\nParsing succeeded\n\n";
        }
        else {
            std::string rest(iter, end);
            std::cout << "\nParsing failed\n\n";
        }
    }
    
    std::cout << "Bye... :-) \n\n";
    return 0;
}
