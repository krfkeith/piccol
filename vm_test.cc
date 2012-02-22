
#include <iostream>
#include <fstream>
#include <streambuf>

#include "nanom.h"
#include "nanom_stringlib.h"
#include "nanom_stdio.h"


int main(int argc, char** argv) {

    std::ifstream t(argv[1]);
    std::string code;

    code.assign(std::istreambuf_iterator<char>(t),
                std::istreambuf_iterator<char>());

    nanom::Vm vm;
    nanom::register_stringlib(vm, 0);
    nanom::register_stdio(vm, 100);

    nanom::Assembler as(vm);
    as.assemble(code);

    as.vm_run("main");

    return 0;
}


