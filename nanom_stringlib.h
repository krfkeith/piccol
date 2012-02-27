#ifndef __NANOM_STRINGLIB_H
#define __NANOM_STRINGLIB_H

#include <stdio.h>

#include "nanom.h"

#include "util.h"

namespace nanom {

namespace {

inline void append_char(Vm& vm) {
    Val cell = vm.pop();
    Val v = vm.pop();

    UInt bm = 0xFF;
    UInt ch = v.uint;
    std::string& s = vm.strtab[cell.uint];
    int offs = 0;

    for (int i = 0; i < 8; ++i) {
        s += (unsigned char)((ch & bm) >> offs);
        bm << 8;
        offs += 8;
    }
}

inline void int_to_string(Vm& vm) {
    Val cell = vm.pop();
    Val v = vm.pop();
    vm.strtab[cell.uint] = nanom::int_to_string(v.inte);
}

inline void uint_to_string(Vm& vm) {
    Val cell = vm.pop();
    Val v = vm.pop();
    vm.strtab[cell.uint] = nanom::uint_to_string(v.uint);
}

inline void real_to_string(Vm& vm) {
    Val cell = vm.pop();
    Val v = vm.pop();
    vm.strtab[cell.uint] = nanom::real_to_string(v.real);
}

inline void string_to_int(Vm& vm) {
    Val cell = vm.pop();
    const std::string& s = vm.strtab[cell.uint];
    vm.push(nanom::string_to_int(s));
}

inline void string_to_uint(Vm& vm) {
    Val cell = vm.pop();
    const std::string& s = vm.strtab[cell.uint];
    vm.push(nanom::string_to_uint(s));
}

inline void string_to_real(Vm& vm) {
    Val cell = vm.pop();
    const std::string& s = vm.strtab[cell.uint];
    vm.push(nanom::string_to_real(s));
}


inline void append_copy_string(Vm& vm) {
    Val cellto = vm.pop();
    Val cellfrom = vm.pop();
    vm.strtab[cellto.uint] += vm.strtab[cellfrom.uint];
}

inline void free_string(Vm& vm) {
    Val cell = vm.pop();
    vm.strtab.erase(cell.uint);
}

}

inline void register_stringlib(Vm& vm, size_t block) {

    vm.register_syscall(1+block, append_char);

    vm.register_syscall(2+block, int_to_string);
    vm.register_syscall(3+block, uint_to_string);
    vm.register_syscall(4+block, real_to_string);

    vm.register_syscall(5+block, string_to_int);
    vm.register_syscall(6+block, string_to_uint);
    vm.register_syscall(7+block, string_to_real);

    vm.register_syscall(8+block, append_copy_string);
    vm.register_syscall(9+block, free_string);
}

inline void register_stringlib(Assembler& as, size_t block) {

    register_stringlib(as.vm, block);

    as.register_const("str_append_char", (UInt)1+block);
    as.register_const("int_to_str", (UInt)2+block);
    as.register_const("uint_to_str", (UInt)3+block);
    as.register_const("real_to_str", (UInt)4+block);
    as.register_const("str_to_int", (UInt)5+block);
    as.register_const("str_to_uint", (UInt)6+block);
    as.register_const("str_to_real", (UInt)7+block);
    as.register_const("str_append", (UInt)8+block);
    as.register_const("str_free", (UInt)9+block);
}

}

#endif
