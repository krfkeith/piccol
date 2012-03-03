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

    Symlist parse(Symlist& code, const Symlist& inp) {

        parser_t::outlist_t out;
        Symlist::list_t unprocessed;
        
        bool ok = parser.parse(code, inp.syms, out, unprocessed);

        if (!ok) {
            
            std::string err;
            for (const auto& s : unprocessed) {
                err += symtab().get(s.sym);
                err += " ";
            }

            throw std::runtime_error("Parse failed. Unconsumed input: " + err);
        }

        Symlist ret;

        for (const auto& n : out) {

            if (n.str != symtab().get("")) {
                ret.syms.push_back(Symcell(Symcell::QATOM, n.str));
            }

            if (!n.capture.empty()) {
                for (const auto& nn : n.capture) {
                    ret.syms.push_back(Symcell(Symcell::QATOM, nn.sym));
                }
            }
        }

        return ret;
    }

    std::string parse(const std::string& code, const std::string& inp) {

        Symlist code_;
        code_.parse(code);

        Symlist sl;
        sl.parse(inp);

        Symlist ret = parse(code_, sl);
        return ret.print();
    }
};

}

#endif
