
#include <iostream>
#include <fstream>
#include <streambuf>

#include "metalan_prime.h"


int main(int argc, char** argv) {

    std::ifstream t(argv[1]);
    std::string code;

    code.assign(std::istreambuf_iterator<char>(t),
                std::istreambuf_iterator<char>());

    std::ifstream t2(argv[2]);
    std::string inp;

    inp.assign(std::istreambuf_iterator<char>(t2),
               std::istreambuf_iterator<char>());


    metalan::NanomAsmProcessor p;
    p.parse(code, inp);

    return 0;
}
