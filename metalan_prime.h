#ifndef __METALAN_PRIME_H
#define __METALAN_PRIME_H


#include "metalan.h"


namespace metalan {


struct charmatcher {

    bool operator()(const Symcell& sc, 
                    std::string::const_iterator& b, 
                    std::string::const_iterator e) {

        if (b == e)
            return false;

        const std::string& str = symtab().get(sc.sym);

        if (sc.type == Symcell::ATOM && str == "*") {
            ++b;
            return true;
        }

        for (unsigned char c : str) {
            if (b == e || c != *b) {
                return false;
            }

            ++b;
        }

        return true;
    }
};



struct MetalanPrime {

    typedef Parser<std::string, charmatcher> parser_t;
    parser_t parser;

    MetalanPrime() {}

    Symlist parse(Symlist& code, const std::string& inp) {

        parser_t::outlist_t out;
        std::string unprocessed;
        
        bool ok = parser.parse(code, inp, out, unprocessed);

        if (!ok) {
            throw std::runtime_error("Parse failed. Unconsumed input: " + unprocessed);
        }

        Symlist ret;

        for (const auto& n : out) {

            const std::string& symstr = symtab().get(n.str);

            // HACK

            if (n.type == 0) {

                if (symstr == "append" && !ret.syms.empty()) {
                    
                    Symcell& prev = ret.syms.back();
                    prev.sym = symtab().get(symtab().get(prev.sym) + n.capture);
                    continue;

                } else if (symstr == "combine" && ret.syms.size() >= 2) {
                    
                    Symcell prev = ret.syms.back();
                    ret.syms.pop_back();
                    Symcell& prevprev = ret.syms.back();
                    prevprev.sym = symtab().get(symtab().get(prevprev.sym) + 
                                                symtab().get(prev.sym));
                    continue;
                }

                ret.syms.push_back(Symcell(Symcell::QATOM, n.capture));

            } else {

                ret.syms.push_back(Symcell(Symcell::QATOM, n.str));
            }
        }
        
        return ret;
    }

    std::string parse(const std::string& code, const std::string& inp) {

        Symlist code_;
        code_.parse(code);

        Symlist ret = parse(code_, inp);
        return ret.print();
    }
};

}

#endif
