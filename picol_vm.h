#ifndef __PICOL_VM_H
#define __PICOL_VM_H

#include "nanom_asm.h"

#include "metalan_prime.h"
#include "metalan_doppel.h"

namespace picol {

struct Picol {

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
            stage1 = prime.parse(lexer, inp);

        } catch (std::exception& e) {
            throw std::runtime_error(std::string("Error in stage 1 (picol_lex): ") + e.what());
        }

        //std::cout << "[" << stage1.print() << "]" << std::endl;

        metalan::Symlist stage2;

        while (1) {
            try {
                // Make a copy of the ruleset, since Metalan::parse will clobber the 
                // ruleset argument.

                metalan::Symlist tmp = morpher;                
                stage2 = doppel.parse(tmp, stage1);

            } catch (std::exception& e) {
                throw std::runtime_error(std::string("Error in stage 2 (picol_morph): ") + e.what());
            }

            //std::string tmp = stage2.print();
            //std::cout << "------------------------------------------" << std::endl;
            //std::cout << tmp << std::endl;

            if (stage1 == stage2) 
                break;

            stage1 = stage2;
        }

        try {
            stage2 = doppel.parse(emiter, stage2);

        } catch (std::exception& e) {
            throw std::runtime_error(std::string("Error in stage 3 (picol_emit): ") + e.what());
        }

        std::string tmp = stage2.print();
        //std::cout << "==============================================" << std::endl;
        //std::cout << tmp << std::endl;

        nanom::VmAsm as;
        as.parse(stage2);

        //std::cout << "++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
        //std::cout << as.print() << std::endl;

        nanom::Vm vm(as.code);
        nanom::vm_run(vm, metalan::symtab().get(""));
    }

};

}

#endif
