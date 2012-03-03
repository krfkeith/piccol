#ifndef __PICOLOL_VM_H
#define __PICOLOL_VM_H

#include "nanom_asm.h"

#include "metalan_prime.h"
#include "metalan_doppel.h"

namespace picolol {

struct Picolol {

    void load(const std::string& lexer_, const std::string& parser_, const std::string& inp) {

        metalan::MetalanPrime prime;
        metalan::MetalanDoppel doppel;

        metalan::Symlist lexer;
        lexer.parse(lexer_);

        metalan::Symlist parser;
        parser.parse(parser_);

        metalan::Symlist stage1 = prime.parse(lexer, inp);

        //std::cout << "[" << stage1.print() << std::endl;

        metalan::Symlist stage2 = doppel.parse(parser, stage1);

        std::string tmp = stage2.print();

        std::cout << tmp << std::endl;

        nanom::VmAsm as;
        as.parse(stage2);
    }

};

}

#endif
