
#include <iostream>
#include <fstream>
#include <streambuf>

#include "metalan_doppel.h"


int main(int argc, char** argv) {

    int fn = 1;
    bool recursive = false;

    if (argc == 3) {
        if (std::string(argv[1]) == "-z") {
            recursive = true;
            fn = 2;

        } else if (std::string(argv[2]) == "-z") {
            recursive = true;
            fn = 1;
        }
    }

    std::ifstream t(argv[fn]);
    std::string code;

    code.assign(std::istreambuf_iterator<char>(t),
                std::istreambuf_iterator<char>());

    //std::ifstream t2(argv[2]);
    std::string inp;

    inp.assign(std::istreambuf_iterator<char>(std::cin),
               std::istreambuf_iterator<char>());

    while (1) {

        metalan::MetalanDoppel p;
        p.parser.verbose = true;

        std::string ret = p.parse(code, inp);

        if (!recursive || ret == inp) {
            std::cout << ret << std::endl;
            break;
        }

        inp = ret;
    }

    return 0;
}
