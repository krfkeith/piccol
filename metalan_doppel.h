#ifndef __METALAN_DOPPEL_H
#define __METALAN_DOPPEL_H


#include "metalan.h"


namespace metalan {

typedef Symlist::list_t::const_iterator symlist_iter;


struct symcellmatcher {

    bool operator()(const Symcell& sc, symlist_iter& b, symlist_iter e) {

        if (b == e) return false;

        if (sc.type == Symcell::ATOM && sc.sym == symtab().get("*")) {
            ++b;
            return true;
        }

        if (b->sym != sc.sym) {
            return false;
        }

        ++b;
        return true;
    }
};



struct MetalanDoppel {

    typedef Parser<Symlist::list_t, symcellmatcher> parser_t;
    parser_t parser;

    MetalanDoppel() {}

    void parse(const std::string& code, const std::string& inp) {

        Symlist sl;
        sl.parse(inp);

        parser_t::outlist_t out;
        Symlist::list_t unprocessed;
        
        bool ok = parser.parse(code, sl.syms, out, unprocessed);

        if (!ok) {
            
            std::string err;
            for (const auto& s : unprocessed) {
                err += symtab().get(s.sym);
                err += " ";
            }

            throw std::runtime_error("Parse failed. Unconsumed input: " + err);
        }

        std::string sout;

        for (const auto& n : out) {

            if (n.str != symtab().get("")) {
                sout += "'";
                sout += symtab().get(n.str);
                sout += "'";
            }

            if (!n.capture.empty()) {
                for (const auto& nn : n.capture) {
                    sout += " '";
                    sout += symtab().get(nn.sym);
                    sout += "'";
                }
            }

            sout += "\n";
        }

        std::cout << sout << std::endl;
    }
};

}

#endif
