#ifndef __PICCOL_VM_H
#define __PICCOL_VM_H

#include "nanom_asm.h"

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

    void register_callback(const std::string& obj, nanom::syscall_callback_t cb) {
        vm.register_callback(metalan::symtab().get(obj), cb);
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
            std::string msg = e.what();

            if (msg.size() > 200) {
                msg.resize(200); 
                msg += "...";
            }

            throw std::runtime_error(std::string("Error in stage 1 (piccol_lex): ") + msg);
        }

        //std::cout << "[" << stage1.print() << "]" << std::endl;

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

            //std::string tmp = stage2.print();
            //std::cout << "------------------------------------------" << std::endl;
            //std::cout << tmp << std::endl;

            if (stage1 == stage2) 
                break;

            stage1 = stage2;
        }

        try {
            bm _b("emiting");
            stage2 = doppel.parse(emiter, stage2);

        } catch (std::exception& e) {
            throw std::runtime_error(std::string("Error in stage 3 (piccol_emit): ") + e.what());
        }

        //std::string tmp = stage2.print();
        //std::cout << "==============================================" << std::endl;
        //std::cout << tmp << std::endl;

        bm _b("assembling");
        as.parse(stage2);

        //std::cout << "++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
        //std::cout << as.print() << std::endl;

        bm _b2("running");
        nanom::vm_run(vm);
    }

};

}

#endif
