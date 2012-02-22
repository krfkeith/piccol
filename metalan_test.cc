
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

    inp.assign(std::istreambuf_iterator<char>(t),
               std::istreambuf_iterator<char>());


    metalan::Parser parser;
    parser.parse(code, inp);

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

    return 0;
}
