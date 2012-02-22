
#include <iostream>
#include <fstream>
#include <streambuf>

#include "metalan.h"

int main(int argc, char** argv) {

    std::ifstream t(argv[1]);
    std::string code;

    code.assign(std::istreambuf_iterator<char>(t),
                std::istreambuf_iterator<char>());

    std::ifstream t2(argv[2]);
    std::string inp;

    inp.assign(std::istreambuf_iterator<char>(t2),
               std::istreambuf_iterator<char>());


    metalan::Parser parser;

    std::string out;
    bool ok = parser.parse(code, inp, out);

    if (!ok) {
        throw std::runtime_error("Parse failed. Unconsumed input: " + out);
    }

    std::cout << out << std::endl;

    /*
    for (auto& cc : parser.rules) {
        for (auto& rl : cc.second) {
            std::cout << metalan::symtab().get(cc.first) << " :-";
            for (auto& cell : rl) {
                std::cout << " " << metalan::symtab().get(cell.sym) << "(" << cell.type << ")";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    */

    return 0;
}
