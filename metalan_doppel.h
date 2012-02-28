#ifndef __METALAN_DOPPEL_H
#define __METALAN_DOPPEL_H


#include "metalan.h"


namespace metalan {

typedef Symlist::list_t::const_iterator symlist_iter;


struct symcellcapture {

    void operator()(std::string& s, 
                    symlist_iter b, 
                    symlist_iter e) {

        while (b != e) {
            if (!s.empty()) {
                s += " ";
            }

            s += symtab().get(b->sym);
            ++b;
        }
    }
};


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

    Parser<Symlist::list_t, symcellmatcher, symcellcapture> parser;

    MetalanDoppel() {}

    void parse(const std::string& code, const std::string& inp) {

        Symlist sl;
        sl.parse(inp);

        Outlist out;
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

            sout += "'";
            sout += n.str;
            sout += "'";

            if (!n.capture.empty()) {
                sout += ":'";
                sout += n.capture;
                sout += "'";
            }

            sout += "\n";
        }

        std::cout << sout << std::endl;
    }
};

}

#endif
