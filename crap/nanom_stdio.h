#ifndef __NANOM_STDIO_H
#define __NANOM_STDIO_H

#include <iostream>

#include "nanom.h"

namespace nanom {

namespace {

inline void print(Vm& vm) {
    Val cell = vm.pop();
    std::cout << vm.strtab[cell.uint] << std::endl;
}

}

inline void register_stdio(Vm& vm, size_t block) {

    vm.register_syscall(1+block, print);
}

inline void register_stdio(Assembler& as, size_t block) {
    
    register_stdio(as.vm, block);

    as.register_const("print", (UInt)(1+block));
}

}

#endif
