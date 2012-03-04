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

        metalan::Symlist stage1 = prime.parse(lexer, inp);

        std::cout << "[" << stage1.print() << "]" << std::endl;

        metalan::Symlist stage2;

        while (1) {
            stage2 = doppel.parse(morpher, stage1);

            std::string tmp = stage2.print();
            std::cout << "------------------------------------------" << std::endl;
            std::cout << tmp << std::endl;

            if (stage1 == stage2) 
                break;

            stage1 = stage2;
        }

        stage2 = doppel.parse(emiter, stage2);

        std::string tmp = stage2.print();
        std::cout << "==============================================" << std::endl;
        std::cout << tmp << std::endl;


        nanom::VmAsm as;
        as.parse(stage2);
    }

};

}

#endif
