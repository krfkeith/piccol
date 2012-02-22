#ifndef __CROMLECH_META_H
#define __CROMLECH_META_H

#include <cstdint>
#include <stdexcept>

#include <string>
#include <list>
#include <unordered_map>

#include <iostream>
#include <fstream>
#include <streambuf>



namespace crom {

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
        VAR
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

    std::list<Symcell> syms;

    void parse(const std::string& s) {

        std::string buff;
        std::string::const_iterator i = s.begin();
        enum {
            IN_BLANK,
            IN_ATOM,
            IN_VAR
        } state = IN_BLANK;

        while (1) {

            if (i == s.end()) break;

            switch (state) {
            case IN_ATOM: {
                
                if (*i == '\'') {
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
                    syms.push_back(Symcell(Symcell:VAR, buff));
                    buff.clear();
                    state = IN_BLANK;

                } else {
                    syms.push_back(Symcell(Symcell:VAR, buff));
                    buff.clear();

                    syms.push_back(Symcell(Symcell::ATOM, std::string(1, *i)));
                    state = IN_BLANK;
                }
            }

            case IN_BLANK: {

                if (*i == ' ' || *i == '\t' || *i == '\n' || *i == '\r') {
                    // Nothing.

                } else if (*i == '\'') {
                    state = IN_ATOM;

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

        } else if (state == IN_ATOM) {
            throw std::runtime_error("Unterminated \"'\" while at end-of-input.");
        }
    }
};


inline std::string read_file(const std::string& file) {

    std::ifstream t("file.txt");
    std::string str(std::istreambuf_iterator<char>(t),
                    std::istreambuf_iterator<char>());
    return str;
}


}

#endif

/*

----

space :- ' '; '\n'; '\t'.

spaces :- space spaces.
spaces :- .

digit :- 0 { PUSH_INT(0) }.
digit :- 1 { PUSH_INT(1) }.
digit :- 2 { PUSH_INT(2) }.
digit :- 3 { PUSH_INT(3) }.
digit :- 4 { PUSH_INT(4) }.
digit :- 5 { PUSH_INT(5) }.
digit :- 6 { PUSH_INT(6) }.
digit :- 7 { PUSH_INT(7) }.
digit :- 8 { PUSH_INT(8) }.
digit :- 9 { PUSH_INT(9) }.

int1 :- { PUSH_INT(10); MUL; } digit { ADD; } int1.
int1 :- .

int :- digit int1.
int :- digit.

elt :- ( expr ).
elt :- - int.
elt :- int.

expr_m :- elt * expr_m.
expr_m :- elt / expr_m.
expr_m :- elt.

expr_a :- expr_m + expr_a.
expr_a :- expr_m - expr_a.
expr_a :- expr_m.

expr :- spaces expr_a spaces.

all :- expr all.
all :- expr.

:- all.

