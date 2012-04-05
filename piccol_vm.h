#ifndef __PICCOL_VM_H
#define __PICCOL_VM_H

#include <initializer_list>

#include "piccol_asm.h"

#include "metalan_prime.h"
#include "metalan_doppel.h"
#include "macrolan.h"

#include <iostream>
#include <fstream>
#include <streambuf>


#include <sys/time.h>
struct bm {
    struct timeval b;
    std::string msg;
    bm(const std::string& s) : msg(s) {
        gettimeofday(&b, NULL);
    }
    ~bm() {
        struct timeval e;
        gettimeofday(&e, NULL);
        size_t a = (e.tv_sec*1e6 + e.tv_usec);
        size_t q = (b.tv_sec*1e6 + b.tv_usec);
        std::cout << msg << ": " << ((double)a-(double)q)/1e6 << std::endl;
    }
};


namespace piccol {

inline std::string load_file(const std::string& fname) {

    std::ifstream file(fname);

    if (!file)
        throw std::runtime_error("Could not open '" + fname + "'");

    std::string ret;

    ret.assign(std::istreambuf_iterator<char>(file),
               std::istreambuf_iterator<char>());
    return ret;
}

struct Piccol {

    nanom::VmCode code;
    nanom::Vm vm;
    PiccolAsm as;
    macrolan::Macrolan macro;

    std::string macro_code;
    std::string lexer_code;
    std::string morpher_code;
    std::string emiter_code;
    std::string prelude_code;

    Piccol(std::string&& macrolan_,
           std::string&& lexer_, 
           std::string&& morpher_,
           std::string&& emiter_,
           std::string&& prelude_) : 
        vm(code), as(vm), macro(macrolan_),
        macro_code(macrolan_),
        lexer_code(lexer_),
        morpher_code(morpher_),
        emiter_code(emiter_),
        prelude_code(prelude_)
        {}

    void register_callback(const std::string& name, const std::string& from, const std::string& to,
                           nanom::callback_t cb) {
        code.register_callback(nanom::label_t(metalan::symtab().get(name),
                                              metalan::symtab().get(from), 
                                              metalan::symtab().get(to)), 
                               cb);
    }

    void init() {
        load(prelude_code);
    }

    void load(const std::string& inp_) {

        std::string inp;

        try {
            inp = macro.parse(inp_);

        } catch (std::exception& e) {
            throw std::runtime_error(std::string("Error in pre-processing: ") + e.what());
        }

        metalan::MetalanPrime prime;
        metalan::MetalanDoppel doppel;

        metalan::Symlist lexer;
        lexer.parse(lexer_code);

        metalan::Symlist morpher;
        morpher.parse(morpher_code);

        metalan::Symlist emiter;
        emiter.parse(emiter_code);

        metalan::Symlist stage1;

        try {
            bm _b("parsing");
            stage1 = prime.parse(lexer, inp);

        } catch (std::exception& e) {
            throw std::runtime_error(std::string("Error in stage 1 (piccol_lex): ") + e.what());
        }

        metalan::Symlist stage2;

        while (1) {
            try {
                // Make a copy of the ruleset, since Metalan::parse will clobber the 
                // ruleset argument.

                bm _b("transformation");
                metalan::Symlist tmp = morpher;
                stage2 = doppel.parse(tmp, stage1);

            } catch (std::exception& e) {
                throw std::runtime_error(std::string("Error in stage 2 (piccol_morph): ") + e.what());
            }

            if (stage1 == stage2) 
                break;

            stage1 = stage2;
        }

        //std::cout << "++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
        //std::cout << stage2.print() << std::endl;

        try {
            bm _b("emiting");
            stage2 = doppel.parse(emiter, stage2);

        } catch (std::exception& e) {
            throw std::runtime_error(std::string("Error in stage 3 (piccol_emit): ") + e.what());
        }

        //std::cout << "++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
        //std::cout << stage2.print() << std::endl;

        bm _b("assembling");
        as.parse(stage2);

        //std::cout << "-----------------" << std::endl;
        //std::cout << as.print() << std::endl;

        vm.reset();
    }

    bool run(metalan::Sym name, metalan::Sym s1, metalan::Sym s2, 
             const nanom::Struct& in, nanom::Struct& out) {

        size_t stack_size = vm.stack.size();
        vm.stack.insert(vm.stack.end(), in.v.begin(), in.v.end());

        return run(name, s1, s2, out, stack_size);
    }

    bool run(metalan::Sym name, metalan::Sym s1, metalan::Sym s2, nanom::Struct& out, size_t framehead) {
        bm _b("running");

        nanom::label_t l(name, s1, s2);

        if (vm.code.codes.find(l) == vm.code.codes.end()) {
            throw std::runtime_error("Undefined function: " + l.print());
        }

        vm.frame.emplace_back(l, 0, framehead, vm.stack.size() - framehead);

        vm.failbit = false;
        nanom::vm_run(vm, l); //, 0, true);

        out.v.assign(vm.stack.begin() + framehead, vm.stack.end());
        vm.stack.erase(vm.stack.begin() + framehead, vm.stack.end());

        bool ret = !(vm.failbit);
        return ret;
    }

    bool run(const std::string& name, const std::string& fr, const std::string& to, nanom::Struct& out) {
        return run(metalan::symtab().get(name), metalan::symtab().get(fr), metalan::symtab().get(to),
                   out, vm.stack.size());
    }

    bool run(const std::string& name, const std::string& fr, const std::string& to, 
             const nanom::Struct& in, nanom::Struct& out) {

        return run(metalan::symtab().get(name), metalan::symtab().get(fr), metalan::symtab().get(to),
                   in, out);
    }

    void check_type(const std::string& shape, std::initializer_list<nanom::Type> types) {

        bool ok = false;

        if (code.shapes.has_shape(metalan::symtab().get(shape))) {
            
            const Shape& sh = code.shapes.get(shape);

            if (sh.serialized.size() == types.size()) {
                
                if (std::equal(sh.serialized.begin(), sh.serialized.end(), types.begin())) {
                    ok = true;
                }
            }
        }

        if (!ok) {

            std::string sig = "[ ";
            for (auto t : types) {
                switch (t) {
                case BOOL: sig += "Bool "; break;
                case SYMBOL: sig += "Sym "; break;
                case INT: sig += "Int "; break;
                case UINT: sig += "UInt "; break;
                case REAL: sig += "Real "; break;
                default: break;
                }
            }
            sig += "]";                    

            throw std::runtime_error("A type named " +
                                     shape + " with signature " +
                                     sig + " is required");
        }
    }

};

}

#endif
