#ifndef __NANOM_SYMTABLIB_H
#define __NANOM_SYMTABLIB_H

#include <stdio.h>

#include "nanom.h"

#include "util.h"

namespace nanom {

namespace {

struct symtab_t {
    std::unordered_map<std::string,size_t> syms;
};

struct symtabs_t {
    std::unordered_map<size_t,symtab_t> syms;

    size_t get(size_t i, const std::string& s) {
        symtab_t& ss = syms[i];

        auto ii = ss.syms.find(s);
        if (ii != ss.syms.end()) {
            return ii->second;
        }

        size_t ret = ss.syms.size()+1;
        ss.syms.insert(ii, std::make_pair(s, ret));
        return ret;
    }
};

inline symtabs_t& symtabs() {
    static symtabs_t ret;
    return ret;
}

inline void get_sym(Vm& vm) {
    Val symcell = vm.pop();
    Val strcell = vm.pop();
    const std::string& s = vm.strtab[strcell.uint];

    if (s.empty()) {
        throw std::runtime_error("Empty string cannot be a symbol");
    }

    vm.push((UInt)symtabs().get(symcell.uint, s));
}

}

inline void register_symtablib(Vm& vm, size_t block) {
    vm.register_syscall(1+block, get_sym);
}

inline void register_symtablib(Assembler& as, size_t block) {

    register_symtablib(as.vm, block);

    as.register_const("symtab_get", (UInt)1+block);
}

}

#endif
