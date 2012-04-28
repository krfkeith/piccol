#ifndef __METALAN_PRIME_H
#define __METALAN_PRIME_H


#include "metalan.h"

#include <ctype.h>

namespace metalan {


struct charmatcher {

    bool operator()(const Symcell& sc, 
                    std::string::const_iterator& b, 
                    std::string::const_iterator e, size_t& n) {

        static Sym end = symtab().get("end");
        static Sym any = symtab().get("any");
        static Sym digit = symtab().get("digit");
        static Sym locase = symtab().get("locase");
        static Sym upcase = symtab().get("upcase");

        //std::cout << metalan::symtab().get(sc.sym) << "~[" << std::string(b, e).substr(0, 10)
        //          << "]" << std::endl;

        if (b == e) {
            if (sc.type == Symcell::ESCAPE && sc.sym == end) {
                return true;
            }

            return false;
        }

        if (sc.type == Symcell::ESCAPE) {

            if (sc.sym == any) {
                ++b;
                ++n;
                return true;

            } else if (sc.sym == digit) {
                if (::isdigit(*b)) {
                    ++b;
                    ++n;
                    return true;
                } else {
                    return false;
                }

            } else if (sc.sym == locase) {
                if (::islower(*b)) {
                    ++b;
                    ++n;
                    return true;
                } else {
                    return false;
                }

            } else if (sc.sym == upcase) {
                if (::isupper(*b)) {
                    ++b;
                    ++n;
                    return true;
                } else {
                    return false;
                }
            }
        }

        const std::string& str = symtab().get(sc.sym);

        for (unsigned char c : str) {
            if (b == e || c != *b) {
                return false;
            }

            ++b;
            ++n;
        }

        return true;
    }
};



struct MetalanPrime {

    typedef Parser<std::string, charmatcher> parser_t;

    MetalanPrime() {}

    Symlist parse(Symlist& code, const std::string& inp, 
                  bool verbose = false, const std::string& mainrule = "main") {

        parser_t parser;
        parser.verbose = verbose;

        parser_t::outlist_t out;
        std::string unprocessed;
        
        bool ok = parser.parse(code, inp, out, unprocessed, mainrule);

        if (!ok) {
            std::string ext = std::string(parser.largest_extent, inp.end());

            if (ext.size() > 20)
                ext.resize(20);

            if (unprocessed.size() > 50)
                unprocessed.resize(50);

            throw std::runtime_error("Parse failed at:\n<<<\n" + ext +
                                     "\n>>>\nUnconsumed input:\n<<<\n" + unprocessed +
                                     "\n>>>\n");
        }

        Symlist ret;

        std::vector<std::string> capture_stack;

        /*
         * Special commands:
         * 
         * 'append'      : Append the current capture to the top of the stack.
         * 'combine'     : Append the previously emitted symbol to the top of the stack.
         *                 The current capture will be ignored, the previously emitted symbol
         *                 will be deleted.
         * 'combine_keep': Like 'combine', but the previously emitted symbol will _not_ be
         *                 deleted.
         * 'push'        : Push current capture on a stack, do not emit any symbols.
         * 'pop'         : Pop a capture from the stack and emit the symbols is contains. 
         *                 The current capture will be ignored.
         * 'top'         : Emit the symbols in the top capture on the stack.
         *                 The current capture will be ignored.
         * 'rot'         : Swap the two top stack elements, do not emit anything.
         */
        

        for (const auto& n : out) {

            const std::string& symstr = symtab().get(n.str);

            // HACK

            if (n.type == 0) {

                if (symstr == "append") {
                    
                    if (capture_stack.empty())
                        continue;

                    capture_stack.back() += n.capture;
                    continue;

                } else if (symstr == "combine" && ret.syms.size() >= 1) {

                    if (capture_stack.empty())
                        continue;
                    
                    Symcell prev = ret.syms.back();
                    ret.syms.pop_back();
                    capture_stack.back() += symtab().get(prev.sym);
                    continue;

                } else if (symstr == "combine_keep" && ret.syms.size() >= 1) {

                    if (capture_stack.empty())
                        continue;
                    
                    Symcell prev = ret.syms.back();
                    capture_stack.back() += symtab().get(prev.sym);
                    continue;

                } else if (symstr == "push") {

                    capture_stack.push_back(n.capture);
                    continue;

                } else if (symstr == "pop") {

                    if (capture_stack.empty())
                        continue;

                    ret.syms.push_back(Symcell(Symcell::QATOM, capture_stack.back()));
                    capture_stack.pop_back();
                    continue;

                } else if (symstr == "top") {

                    if (capture_stack.empty())
                        continue;

                    ret.syms.push_back(Symcell(Symcell::QATOM, capture_stack.back()));
                    continue;

                } else if (symstr == "rot") {
                    if (capture_stack.size() < 2)
                        continue;

                    std::string& s1 = capture_stack.back();
                    std::string& s2 = capture_stack[capture_stack.size()-2];
                    s1.swap(s2);
                }

                ret.syms.push_back(Symcell(Symcell::QATOM, n.capture));

            } else {

                ret.syms.push_back(Symcell(Symcell::QATOM, n.str));
            }
        }
        
        return ret;
    }

    std::string parse(const std::string& code, const std::string& inp, bool raw, 
                      bool verbose = false, const std::string& mainrule = "main") {

        Symlist code_;
        code_.parse(code);

        Symlist ret = parse(code_, inp, verbose, mainrule);

        if (raw) {
            return ret.print_raw();

        } else {

            return ret.print();
        }
    }
};

}

#endif
