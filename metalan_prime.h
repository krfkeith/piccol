#ifndef __METALAN_PRIME_H
#define __METALAN_PRIME_H


#include "metalan.h"


namespace metalan {


struct charmatcher {

    bool operator()(const Symcell& sc, 
                    std::string::const_iterator& b, 
                    std::string::const_iterator e) {

        const std::string& str = symtab().get(sc.sym);

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

    std::string parse(const std::string& code, const std::string& inp) {

        parser_t::outlist_t out;
        std::string unprocessed;
        
        bool ok = parser.parse(code, inp, out, unprocessed);

        if (!ok) {
            throw std::runtime_error("Parse failed. Unconsumed input: " + unprocessed);
        }

        std::string sout;

        for (const auto& n : out) {

            sout += "'";
            sout += symtab().get(n.str);
            sout += "'";

            if (!n.capture.empty()) {
                sout += ":'";
                sout += n.capture;
                sout += "'";
            }

            sout += "\n";
        }

        return sout;
    }
};

}

#endif
