
#include <iostream>
#include <fstream>
#include <streambuf>

#include "metalan_prime.h"


int main(int argc, char** argv) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <metalan source file>" << std::endl;
        return 1;
    }

    std::ifstream t(argv[1]);
    std::string code;

    code.assign(std::istreambuf_iterator<char>(t),
                std::istreambuf_iterator<char>());

    std::string inp;

    inp.assign(std::istreambuf_iterator<char>(std::cin),
               std::istreambuf_iterator<char>());


    metalan::MetalanPrime p;

    std::string ret = p.parse(code, inp, true);

    std::cout << ret << std::endl;

    return 0;
}
