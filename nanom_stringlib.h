#ifndef __NANOM_STRINGLIB_H
#define __NANOM_STRINGLIB_H

#include <stdio.h>

#include "nanom.h"

namespace nanom {

namespace {

inline void append_char(Vm& vm) {
    Val cell = vm.pop();
    Val v = vm.pop();

    UInt bm = 0xFF;
    UInt ch = v.uint;
    std::string& s = vm.symtab[cell.uint];
    int offs = 0;

    for (int i = 0; i < 8; ++i) {
        s += (unsigned char)((ch & bm) >> offs);
        bm << 8;
        offs += 8;
    }
}

inline void int_to_string(Vm& vm) {
    static char buff[1024];
    Val cell = vm.pop();
    Val v = vm.pop();
    ::snprintf(buff, 1023, "%lld", v.inte);
    vm.symtab[cell.uint] = buff;
}

inline void uint_to_string(Vm& vm) {
    static char buff[1024];
    Val cell = vm.pop();
    Val v = vm.pop();
    ::snprintf(buff, 1023, "%llu", v.uint);
    vm.symtab[cell.uint] = buff;
}

inline void real_to_string(Vm& vm) {
    static char buff[1024];
    Val cell = vm.pop();
    Val v = vm.pop();
    ::snprintf(buff, 1023, "%g", v.real);
    vm.symtab[cell.uint] = buff;
}

inline void append_copy_string(Vm& vm) {
    Val cellfrom = vm.pop();
    Val cellto = vm.pop();
    vm.symtab[cellto.uint] += vm.symtab[cellfrom.uint];
}

inline void free_string(Vm& vm) {
    Val cell = vm.pop();
    vm.symtab.erase(cell.uint);
}

}

inline void register_stringlib(Vm& vm, size_t block) {

    vm.register_syscall(1+block, append_char);
    vm.register_syscall(2+block, int_to_string);
    vm.register_syscall(3+block, uint_to_string);
    vm.register_syscall(4+block, real_to_string);
    vm.register_syscall(5+block, append_copy_string);
    vm.register_syscall(6+block, free_string);
}

}

#endif
