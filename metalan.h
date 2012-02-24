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
        ACTION_DATA,
        ACTION_CODE
    };

    type_t type;
    Sym sym;

    Symcell(type_t t = ATOM, Sym s = 0) : type(t), sym(s) {}

    Symcell(type_t t = ATOM, std::string s = "") :
        type(t),
        sym(symtab().get(s))
        {}
};


struct Symlist {

    typedef std::list<Symcell> list_t;

    list_t syms;

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


struct Outnode {
    enum type_t {
        CODE,
        DATA
    }; 

    type_t type;
    std::string str;
    std::string capture;

    Outnode(type_t t = DATA, const std::string& s = "") : type(t), str(s) {}
};


typedef std::list<Outnode> Outlist;



struct Parser {

    typedef Symlist::list_t list_t;

    std::unordered_map< Sym, std::vector<list_t> > rules;

    std::unordered_map< Sym, Sym > actions;


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
    }

        
    list_t::iterator process_query(list_t& l, list_t::iterator b, list_t::iterator e) {

        if (b == e)
            throw std::runtime_error("Empty list instead of rule");

        static Sym colon = symtab().get(":");
        static Sym dash = symtab().get("-");
        static Sym dot = symtab().get(".");
        static Sym at = symtab().get("@");
        static Sym amp = symtab().get("&");

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

                    rules[head].push_back(list_t());
                    list_t& tmp = rules[head].back();

                    tmp.splice(tmp.end(), l, rstart, b);
                    
                    ++b;
                    return b;

                } else if (b->sym == at || b->sym == amp) {

                    bool ishead = (b == rstart);
                    bool isdata = (b->sym == at);

                    b = l.erase(b);
                    if (b == e)
                        throw std::runtime_error("Unterminated rule body");

                    if (ishead)
                        rstart = b;

                    if (b->type == Symcell::VAR) {
                        auto i = actions.find(b->sym);

                        if (i == actions.end())
                            throw std::runtime_error("Unknown 'define' referenced: " + 
                                                     symtab().get(b->sym));

                        b->sym = i->second;
                    }

                    b->type = (isdata ? Symcell::ACTION_DATA : Symcell::ACTION_CODE);

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
        }

        return savedb;
    }



    bool apply_one(const list_t& rule, 
                   std::string::const_iterator& b, std::string::const_iterator e, 
                   Outlist& out) {

        std::string::const_iterator savedb = b;
        Outlist subout;

        for (const Symcell& sc : rule) {

            if (sc.type == Symcell::ATOM || sc.type == Symcell::QATOM) {

                const std::string& tomatch = symtab().get(sc.sym);

                std::string::const_iterator mi = tomatch.begin();
                std::string::const_iterator me = tomatch.end();

                while (mi != me) {

                    if (b == e || *b != *mi) {
                        b = savedb;
                        return false;
                    }

                    ++b;
                    ++mi;
                }
                continue;

            } else if (sc.type == Symcell::VAR) {

                if (!apply(symtab().get(sc.sym), b, e, subout)) {
                    b = savedb;
                    return false;
                }

            } else if (sc.type == Symcell::ACTION_DATA) {

                subout.push_back(Outnode(Outnode::DATA, symtab().get(sc.sym)));

            } else if (sc.type == Symcell::ACTION_CODE) {

                subout.push_back(Outnode(Outnode::CODE, symtab().get(sc.sym)));
                subout.back().capture.assign(savedb, b);
            }
        }

        out.splice(out.end(), subout, subout.begin(), subout.end());

        return true;
    }

    bool apply(const std::string& rule, 
               std::string::const_iterator& b, std::string::const_iterator e, 
               Outlist& out) {

        auto it = rules.find(symtab().get(rule));

        if (it == rules.end())
            throw std::runtime_error("Unknown rule referenced: '" + rule + "'");

        for (const list_t& r : it->second) {
            if (apply_one(r, b, e, out)) {
                return true;
            }
        }

        return false;
    }
               

    bool parse(const std::string& pr, const std::string& inp, Outlist& out,
               std::string& unprocessed) {

        Symlist prog;
        prog.parse(pr);

        list_t::iterator i = prog.syms.begin();
        list_t::iterator e = prog.syms.end();

        while (1) {

            if (i == e) break;

            bool did_expand;
            do {
                i = process_directive(prog.syms, i, e, did_expand);
            } while (did_expand);

            i = process_query(prog.syms, i, e);

        }

        std::string::const_iterator sb = inp.begin();
        std::string::const_iterator se = inp.end();

        bool ok = apply("main", sb, se, out);

        if (!ok || sb != se) {
            unprocessed.assign(sb, se);
            return false;
        }

        return true;
    }
};


}

#endif

