
#include <iostream>
#include <fstream>
#include <streambuf>

#include "metalan_prime.h"


int main(int argc, char** argv) {

    std::ifstream t(argv[1]);
    std::string code;

    code.assign(std::istreambuf_iterator<char>(t),
                std::istreambuf_iterator<char>());

    std::string inp;

    inp.assign(std::istreambuf_iterator<char>(std::cin),
               std::istreambuf_iterator<char>());


    metalan::MetalanPrime p;

    bool raw = false;

    if (argc > 2 && argv[2] == std::string("-r")) {
        raw = true;
    }

    std::string ret = p.parse(code, inp, raw);

    std::cout << ret << std::endl;

    return 0;
}
