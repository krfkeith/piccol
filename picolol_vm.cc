
#include <iostream>
#include <fstream>
#include <streambuf>

#include "picolol_vm.h"


int main(int argc, char** argv) {

    std::string inp;

    inp.assign(std::istreambuf_iterator<char>(std::cin),
               std::istreambuf_iterator<char>());

    nanom::VmAsm as;
    as.parse(inp);

    return 0;
}
