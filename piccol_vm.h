#ifndef __PICCOL_VM_H
#define __PICCOL_VM_H

#include "piccol_asm.h"

#include "metalan_prime.h"
#include "metalan_doppel.h"

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

struct Piccol {

    nanom::VmAsm as;
    nanom::Vm vm;

    Piccol() : vm(as.code) {}

    void register_callback(const std::string& name, const std::string& from, const std::string& to,
                           nanom::callback_t cb) {
        vm.register_callback(nanom::label_t(metalan::symtab().get(name),
                                            metalan::symtab().get(from), 
                                            metalan::symtab().get(to)), 
                             cb);
    }

    void load(const std::string& lexer_, 
              const std::string& morpher_,
              const std::string& emiter_, 
              const std::string& inp) {

        metalan::MetalanPrime prime;
        metalan::MetalanDoppel doppel;

        metalan::Symlist lexer;
        lexer.parse(lexer_);

        metalan::Symlist morpher;
        morpher.parse(morpher_);

        metalan::Symlist emiter;
        emiter.parse(emiter_);

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

        bm _b2("running");
        nanom::vm_run(vm, as.nillabel);
    }

    void run(metalan::Sym name, metalan::Sym s1, metalan::Sym s2, nanom::Struct& out) {
        nanom::vm_run(vm, nanom::label_t(name, s1, s2), 0, true);

        // This isn't really needed since we shouldn't exit out of middle of a call stack.

        if (vm.frame.size() > 0) {
            out.v.assign(vm.stack.begin() + vm.frame.back().stack_ix + vm.frame.back().struct_size,
                         vm.stack.end());

        } else {
            out.v.assign(vm.stack.begin(), vm.stack.end());
        }

        vm.stack.clear();
        vm.frame.clear();
    }

    void run(const std::string& name, const std::string& fr, const std::string& to, nanom::Struct& out) {
        run(metalan::symtab().get(name), metalan::symtab().get(fr), metalan::symtab().get(to),
            out);
    }
};

}

#endif
