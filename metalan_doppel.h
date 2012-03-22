#ifndef __METALAN_DOPPEL_H
#define __METALAN_DOPPEL_H


#include "metalan.h"


namespace metalan {

typedef Symlist::list_t::const_iterator symlist_iter;


struct symcellmatcher {

    bool operator()(const Symcell& sc, symlist_iter& b, symlist_iter e, size_t& n) {

        if (b == e) return false;

        if (sc.type == Symcell::ESCAPE && sc.sym == symtab().get("any")) {
            ++b;
            ++n;
            return true;
        }

        if (b->sym != sc.sym) {
            return false;
        }

        ++b;
        ++n;
        return true;
    }
};



struct MetalanDoppel {

    typedef Parser<Symlist::list_t, symcellmatcher> parser_t;

    MetalanDoppel() {}

    Symlist parse(Symlist& code, const Symlist& inp, 
                  bool verbose = false, const std::string& mainrule = "main") {

        parser_t parser;
        parser.verbose = verbose;

        parser_t::outlist_t out;
        Symlist::list_t unprocessed;
        
        bool ok = parser.parse(code, inp.syms, out, unprocessed, mainrule);

        if (!ok) {
            
            Symlist unpr;
            unpr.syms.swap(unprocessed);

            Symlist ext;
            ext.syms.assign(parser.largest_extent, inp.syms.end());

            throw std::runtime_error("Parse failed at:\n<<<\n" + ext.print(15) +
                                     "\n>>>\nUnconsumed input:\n<<<\n" + unpr.print(145) + 
                                     "\n>>>\n");
        }

        Symlist ret;

        std::vector<Symlist::list_t> capture_stack;

        /*
         * Special commands:
         * 
         * 'push' : Push current capture on a stack, do not emit any symbols.
         * 'pop'  : Pop a capture from the stack and emit the symbols is contains. 
         *          The current capture will be ignored.
         * 'drop' : Discard the top capture from the stack, do not emit anything.
         */

        for (auto& n : out) {

            if (n.type == 0) {

                const std::string& symstr = symtab().get(n.str);

                if (symstr == "push") {

                    capture_stack.push_back(Symlist::list_t());
                    capture_stack.back().swap(n.capture);

                } else if (symstr == "pop") {

                    if (capture_stack.empty())
                        continue;
                    
                    for (const auto& nn : capture_stack.back()) {
                        ret.syms.push_back(Symcell(Symcell::QATOM, nn.sym));
                    }

                    capture_stack.pop_back();
                    continue;

                } else if (symstr == "drop") {

                    if (!capture_stack.empty()) {
                        capture_stack.pop_back();
                    }

                    continue;
                }

                for (const auto& nn : n.capture) {
                    ret.syms.push_back(Symcell(Symcell::QATOM, nn.sym));
                }

            } else {
                ret.syms.push_back(Symcell(Symcell::QATOM, n.str));
            }
        }

        return ret;
    }

    std::string parse(const std::string& code, const std::string& inp, 
                      bool verbose = false, const std::string& mainrule = "main") {

        Symlist code_;
        code_.parse(code);

        Symlist sl;
        sl.parse(inp);

        Symlist ret = parse(code_, sl, verbose, mainrule);
        return ret.print();
    }
};

}

#endif
