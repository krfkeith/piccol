#ifndef __METALAN_H
#define __METALAN_H

#include <cstdint>
#include <stdexcept>

#include <string>
#include <vector>
#include <list>
#include <unordered_map>

#include <iostream>
#include <fstream>
#include <streambuf>


namespace {

inline std::string read_file(const std::string& f) {
    std::ifstream t(f);

    if (!t)
        throw std::runtime_error("File not found: " + f);

    std::string ret;
    ret.assign(std::istreambuf_iterator<char>(t),
               std::istreambuf_iterator<char>());
    return ret;
}

}




namespace metalan {

typedef uint64_t Sym;

struct Symtab {

    std::unordered_map<std::string,Sym> _syms;
    std::unordered_map<Sym,std::string> _strs;

    Sym get(const std::string& s) {

        auto i = _syms.find(s);
        if (i != _syms.end()) {
            return i->second;
        }

        Sym sym = _syms.size() + 1;
        _syms[s] = sym;
        _strs[sym] = s;
        return sym;
    }

    const std::string& get(Sym s) {
        static std::string emptystr;

        auto i = _strs.find(s);
        if (i != _strs.end()) {
            return i->second;
        }
        return emptystr;
    }
};

inline Symtab& symtab() {
    static Symtab s;
    return s;
}

struct Symcell {
    enum type_t {
        ATOM,
        QATOM,
        VAR,
        ESCAPE,
        ACTION_DATA,
        ACTION_CODE,
        NEGATE
    };

    type_t type;
    Sym sym;

    Symcell(type_t t = ATOM, Sym s = 0) : type(t), sym(s) {}

    Symcell(type_t t = ATOM, std::string s = "") :
        type(t),
        sym(symtab().get(s))
        {}

    bool operator==(const Symcell& a) const {
        return (sym == a.sym && type == a.type);
    }

    bool operator!=(const Symcell& a) const {
        return !(operator==(a));
    }
};


struct Symlist {

    typedef std::list<Symcell> list_t;

    list_t syms;

    Symlist() {}

    Symlist(const Symlist& sl) : syms(sl.syms) {}

    Symlist(Symlist&& sl) {
        syms.swap(sl.syms);
    }

    Symlist& operator=(const Symlist& sl) {
        syms = sl.syms;
        return *this;
    }

    Symlist& operator=(Symlist&& sl) {
        syms.swap(sl.syms);
        return *this;
    }
    
    bool operator==(const Symlist& a) const {

        list_t::const_iterator b = syms.begin();
        list_t::const_iterator e = syms.end();
        list_t::const_iterator ab = a.syms.begin();
        list_t::const_iterator ae = a.syms.end();


        while (1) {

            if (b == e || ab == ae) {
                if (b == e && ab == ae) {
                    break;
                } else {
                    return false;
                }
            }

            if (*b != *ab) {
                return false;
            }

            ++b;
            ++ab;
        }

        return true;
    }

    std::string print_raw() {
        std::string r0;

        for (const Symcell& s : syms) {
            r0 += symtab().get(s.sym);
        }

        return r0;
    }

    std::string print(size_t nmax=0) {
        std::string ret;
        
        size_t n = 0;
        for (const Symcell& s : syms) {

            if (nmax && n > nmax) {
                break;
            }
            
            if (s.type == Symcell::VAR || s.type == Symcell::ATOM) {
                ret += symtab().get(s.sym);
                ret += "\n";

            } else {
                ret += "'";

                const std::string& symstr = symtab().get(s.sym);
                for (unsigned char cc : symstr) {
                    if (cc == '\'') {
                        ret += "\\'";
                    } else {
                        ret += cc;
                    }
                }

                ret += "'\n";
            }
            ++n;
        }

        return ret;
    }

    void parse(const std::string& s) {

        std::string buff;
        std::string::const_iterator i = s.begin();
        enum {
            IN_BLANK,
            IN_ATOMQ,
            IN_ATOMB,
            IN_VAR
        } state = IN_BLANK;

        while (1) {

            if (i == s.end()) break;

            switch (state) {
            case IN_ATOMQ: 
            case IN_ATOMB: {
                
                if ((state == IN_ATOMQ && *i == '\'') ||
                    (state == IN_ATOMB && *i == '}')) {

                    syms.push_back(Symcell(Symcell::QATOM, buff));
                    buff.clear();
                    state = IN_BLANK;

                } else if (*i == '\\') {
                    ++i;
                    if (i == s.end()) {
                        throw std::runtime_error("Unterminated atom, while parsing: '" + buff + "'");
                    }

                    if (*i == 't') {
                        buff += '\t';

                    } else if (*i == 'n') {
                        buff += '\n';

                    } else {
                        buff += *i;
                    }

                } else {
                    buff += *i;
                }
                break;
            }

            case IN_VAR: {

                if (('A' <= *i && *i <= 'Z') ||
                    ('a' <= *i && *i <= 'z') ||
                    ('0' <= *i && *i <= '9') ||
                    *i == '_') {

                    buff += *i;

                } else if (*i == ' ' || *i == '\t' || *i == '\n' || *i == '\r') {
                    syms.push_back(Symcell(Symcell::VAR, buff));
                    buff.clear();
                    state = IN_BLANK;

                } else {
                    syms.push_back(Symcell(Symcell::VAR, buff));
                    buff.clear();

                    syms.push_back(Symcell(Symcell::ATOM, std::string(1, *i)));
                    state = IN_BLANK;
                }
                break;
            }

            case IN_BLANK: {

                if (*i == ' ' || *i == '\t' || *i == '\n' || *i == '\r') {
                    // Nothing.

                } else if (*i == '\'') {
                    state = IN_ATOMQ;

                } else if (*i == '{') {
                    state = IN_ATOMB;

                } else if (('A' <= *i && *i <= 'Z') ||
                           ('a' <= *i && *i <= 'z') ||
                           *i == '_') {

                    buff += *i;
                    state = IN_VAR;

                } else {
                    syms.push_back(Symcell(Symcell::ATOM, std::string(1, *i)));
                }
                break;
            }
            }

            ++i;
        }


        if (state == IN_VAR) {
            syms.push_back(Symcell(Symcell::VAR, buff));

        } else if (state == IN_ATOMQ || state == IN_ATOMB) {
            throw std::runtime_error("Unterminated \"'\" or \"{\" while at end-of-input.");
        }
    }
};


template <typename TOKENLIST>
struct Outnode {
    enum type_t {
        CODE,
        DATA
    }; 

    type_t type;
    Sym str;
    TOKENLIST capture;

    Outnode(type_t t = DATA, Sym s = "") : type(t), str(s) {}
};


//typedef std::list<Outnode> Outlist;



template <typename TOKENLIST, typename MATCHER>
struct Parser {

    typedef TOKENLIST tokenlist_t;
    typedef typename tokenlist_t::const_iterator tokeniter_t;

    typedef Symlist::list_t list_t;

    typedef Outnode<TOKENLIST> outnode_t;
    typedef std::list<outnode_t> outlist_t;

    struct rule_t {
        list_t common_head;
        std::vector<list_t> alternatives;
        std::vector<bool> tailcallflags;

        void optimize(Sym thisrule) {

            tailcallflags.resize(alternatives.size());

            size_t n = 0;
            for (list_t& l : alternatives) {
                tailcallflags[n] = false;

                if (!l.empty() && l.back().type == Symcell::VAR && l.back().sym == thisrule) {
                    tailcallflags[n] = true;
                    l.pop_back();
                }
                ++n;
            }

            if (alternatives.size() < 2) {
                return;
            }

            typedef std::pair<list_t::iterator,list_t::iterator> range_t;
            std::vector<range_t> alts;

            for (list_t& l : alternatives) {

                if (l.empty()) {
                    return;
                }

                alts.push_back(std::make_pair(l.begin(), l.end()));
            }

            while (1) {
                const Symcell& scprev = *(alts[0].first);

                for (auto& l : alts) {
                    if (*(l.first) != scprev) {
                        return;
                    }
                }

                common_head.push_back(scprev);

                size_t n = 0;
                bool done = false;

                for (auto& l : alts) {
                    list_t::iterator tmp = l.first;
                    ++(l.first);
                    alternatives[n].erase(tmp);

                    if (l.first == l.second) {
                        done = true;
                    }
                    ++n;
                }

                if (done) {
                    return;
                }
            }
        }
    };

    std::unordered_map< Sym, rule_t > rules;

    std::unordered_map< Sym, Sym > actions;


    tokeniter_t largest_extent;
    size_t largest_length;

    bool verbose;


    Parser() : largest_length(0), verbose(false) {}




    Sym consume(Symcell::type_t t, Sym v,
                list_t::iterator& b, list_t::iterator e,
                const std::string& msg) {
        
        if (v != 0) {
            ++b;
            if (b == e) throw std::runtime_error("Premature end of input.");
        }

        if (b->type != t) 
            throw std::runtime_error(msg);

        if (v == 0) return b->sym;

        if (b->sym != v)
            throw std::runtime_error("Expected '" + symtab().get(v) + "' but received '" +
                                     symtab().get(b->sym) + "'");

        return v;
    }

        
    list_t::iterator process_rule(list_t& l, list_t::iterator b, list_t::iterator e) {

        if (b == e)
            throw std::runtime_error("Empty list instead of rule");

        static Sym colon = symtab().get(":");
        static Sym dash = symtab().get("-");
        static Sym dot = symtab().get(".");
        static Sym at = symtab().get("@");
        static Sym amp = symtab().get("&");
        static Sym backslash = symtab().get("\\");
        static Sym exclam = symtab().get("!");

        Sym head = consume(Symcell::VAR, 0, b, e, "Rule head must be a variable");
        consume(Symcell::ATOM, colon, b, e, "Invalid rule syntax");
        consume(Symcell::ATOM, dash, b, e, "Invalid rule syntax");

        ++b;

        list_t::iterator rstart = b;
        while (1) {

            if (b == e)
                throw std::runtime_error("Unterminated rule body");

            if (b->type == Symcell::ATOM) {

                if (b->sym == dot) {

                    rules[head].alternatives.push_back(list_t());
                    list_t& tmp = rules[head].alternatives.back();

                    tmp.splice(tmp.end(), l, rstart, b);
                    
                    ++b;
                    return b;

                } else if (b->sym == at || b->sym == amp || b->sym == backslash || b->sym == exclam) {

                    bool ishead = (b == rstart);
                    bool isat = (b->sym == at);
                    bool isamp = (b->sym == amp);
                    bool isbackslash = (b->sym == backslash);
                    bool isexclam = (b->sym == exclam);

                    b = l.erase(b);
                    if (b == e)
                        throw std::runtime_error("Unterminated rule body");

                    if (ishead)
                        rstart = b;

                    if ((isat || isamp) && b->type == Symcell::VAR) {
                        auto i = actions.find(b->sym);

                        if (i == actions.end())
                            throw std::runtime_error("Unknown 'define' referenced: " + 
                                                     symtab().get(b->sym));

                        b->sym = i->second;

                    } else if (isexclam) {

                        if (b->type != Symcell::VAR || rules.find(b->sym) == rules.end()) 
                            throw std::runtime_error("Invalid rule for the '!' operator: " + symtab().get(b->sym));
                    }

                    if (isat) {
                        b->type = Symcell::ACTION_DATA;

                    } else if (isamp) {
                        b->type = Symcell::ACTION_CODE;

                    } else if (isbackslash) {
                        b->type = Symcell::ESCAPE;

                    } else if (isexclam) {
                        b->type = Symcell::NEGATE;
                    }

                }
            }

            ++b;
        }
    }


    list_t::iterator process_directive(list_t& l, list_t::iterator b, list_t::iterator e, 
                                       bool& did_expand) {

        did_expand = false;

        list_t::iterator savedb = b;

        if (b == e) return savedb;

        if (b->sym != symtab().get(":"))
            return savedb;

        ++b;
        if (b == e) return savedb;

        if (b->sym != symtab().get("-"))
            return savedb;

        ++b;
        if (b == e) return savedb;

        if (b->sym == symtab().get("include")) {

            ++b;
            if (b == e) 
                throw std::runtime_error("End-of-file in 'include' directive");

            std::string f = read_file(symtab().get(b->sym));

            Symlist prog;
            prog.parse(f);

            ++b;

            did_expand = true;

            list_t::iterator ni = l.erase(savedb, b);
            list_t::iterator ret = prog.syms.begin();
            l.splice(ni, prog.syms);

            return ret;

        } else if (b->sym == symtab().get("define")) {

            ++b;
            if (b == e) 
                throw std::runtime_error("End-of-file in 'define' directive");

            Sym name = b->sym;

            ++b;
            if (b == e) 
                throw std::runtime_error("End-of-file in 'define' directive");

            Sym action = b->sym;

            actions[name] = action;

            did_expand = true;

            ++b;
            return b;

        } else if (b->sym == symtab().get("comment")) {

            did_expand = true;

            while (1) {
                if (b->type == Symcell::ATOM && b->sym == symtab().get(".")) {
                    ++b;
                    return b;
                }
                ++b;
            }
        }

        return savedb;
    }



    /*************************************************************/



    bool apply_one(const list_t& rule, const tokeniter_t capturestart,
                   tokeniter_t& b, tokeniter_t e, 
                   outlist_t& out,
                   size_t depth, size_t& length) {

        tokeniter_t savedb = b;
        size_t savedl = length;
        outlist_t subout;

        for (const Symcell& sc : rule) {

            if (sc.type == Symcell::ATOM || sc.type == Symcell::QATOM || sc.type == Symcell::ESCAPE) {

                if (!(MATCHER()(sc, b, e, length))) {
                    b = savedb;
                    length = savedl;
                    return false;
                }

            } else if (sc.type == Symcell::VAR || sc.type == Symcell::NEGATE) {

                bool res = apply(sc.sym, b, e, subout, length, depth + 1);

                if (sc.type == Symcell::NEGATE) {
                    res = !res;

                    if (res) {
                        if (b == e) {
                            res = false;
                        } else {
                            ++b;
                        }
                    }
                }

                if (!res) {
                    b = savedb;
                    length = savedl;
                    return false;
                }

            } else if (sc.type == Symcell::ACTION_DATA) {

                subout.push_back(outnode_t(outnode_t::DATA, sc.sym));

            } else if (sc.type == Symcell::ACTION_CODE) {

                subout.push_back(outnode_t(outnode_t::CODE, sc.sym));

                subout.back().capture.assign(capturestart, b);
            }
        }

        out.splice(out.end(), subout);

        return true;
    }

    bool apply(Sym rule, 
               tokeniter_t& b, tokeniter_t e, 
               outlist_t& out, 
               size_t& length,
               size_t depth = 0) {

        auto it = rules.find(rule);

        if (it == rules.end())
            throw std::runtime_error("Unknown rule referenced: '" + symtab().get(rule) + "'");

        tokeniter_t savedb = b;
        size_t savedl = length;
        outlist_t subout;

      tailcall:

        tokeniter_t capturestart = b;

        if (length >= largest_length) {
            largest_extent = b;
            largest_length = length;
        }

        /**/
        if (verbose) {
            for (size_t i = 0; i < depth; ++i) std::cout << " . ";
            std::cout << "+> " << symtab().get(rule) << ".. :-";
            for (const auto& sc : it->second.common_head)
                std::cout << " " << symtab().get(sc.sym);
            std::cout << std::endl;
        }
        /**/

        if (!apply_one(it->second.common_head, capturestart, b, e, subout, 
                       depth, length)) {

            /**/
            if (verbose) {
                for (size_t i = 0; i < depth; ++i) std::cout << " . ";
                std::cout << "xx " << symtab().get(rule) << std::endl;
            }
            /**/

            return false;
        }

        size_t n = 0;
        for (const list_t& r : it->second.alternatives) {

            /**/
            if (verbose) {
                for (size_t i = 0; i < depth; ++i) std::cout << " . ";
                std::cout << "+- .." << symtab().get(rule) << " :-";
                for (const auto& sc : r)
                    std::cout << " " << symtab().get(sc.sym);
                std::cout << std::endl;
            }
            /**/

            if (apply_one(r, capturestart, b, e, subout, depth, length)) {

                out.splice(out.end(), subout);

                /**/
                if (verbose) {
                    for (size_t i = 0; i < depth; ++i) std::cout << " . ";
                    std::cout << "<< ok " << symtab().get(rule) << std::endl;
                }
                /**/

                if (it->second.tailcallflags[n]) {
                    goto tailcall;
                }

                return true;
            }

            ++n;
        }

        b = savedb;
        length = savedl;

        /**/
        if (verbose) {
            for (size_t i = 0; i < depth; ++i) std::cout << " . ";
            std::cout << "xx failed " << symtab().get(rule) << std::endl;
        }
        /**/

        return false;
    }
               
    bool parse(Symlist& pr, const tokenlist_t& inp, outlist_t& out,
               tokenlist_t& unprocessed, const std::string& mainrule = "main") {

        list_t::iterator i = pr.syms.begin();
        list_t::iterator e = pr.syms.end();

        while (1) {

            if (i == e) break;

            bool did_expand;
            do {
                i = process_directive(pr.syms, i, e, did_expand);
            } while (did_expand);

            i = process_rule(pr.syms, i, e);
        }

        for (auto& r : rules) {
            r.second.optimize(r.first);
        }

        tokeniter_t sb = inp.begin();
        tokeniter_t se = inp.end();

        largest_extent = sb;
        largest_length = 0;

        size_t length = 0;
        bool ok = apply(symtab().get(mainrule), sb, se, out, length);

        if (!ok || sb != se) {
            unprocessed.assign(sb, se);
            return false;
        }

        return true;
    }

    bool parse(const std::string& pr, const tokenlist_t& inp, outlist_t& out,
               tokenlist_t& unprocessed, const std::string& mainrule = "main") {

        Symlist prog;
        prog.parse(pr);

        return parse(prog.syms, inp, out, unprocessed, mainrule);
    }
};


}

#endif

