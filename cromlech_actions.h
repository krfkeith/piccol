#ifndef __CROMLECH_ACTIONS_H
#define __CROMLECH_ACTIONS_H

#include <stdlib.h>

#include <vector>

#include <pegtl.hh>


namespace crom {

using namespace pegtl;

enum {
    LITERAL = 1
};

struct Token {
    static const int type = LITERAL;
    PODLiteral val;
    
    Token() {}
    
    template <typename T>
    Token(const T& t) : val(t) {}
};

typedef std::vector<Token> stack_t;

struct a_int_literal : action_base<a_int_literal> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(::atol(s.c_str())));
    }
};

struct a_real_literal : action_base<a_real_literal> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(::atof(s.c_str())));
    }
};

struct a_symbol_literal : action_base<a_symbol_literal> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(sym(s)));
    }
};

struct a_bool_literal : action_base<a_bool_literal> {
    static void apply(const std::string& s, stack_t& t) {
        if (s == "\\true") {
            t.push_back(Token(true));
        } else {
            t.push_back(Token(false));
        }
    }
};

struct a_string_literal : action_base<a_string_literal> {
    static void apply(const std::string& s, stack_t& t) {
        std::string tmp;

        for (std::string::const_iterator i = s.begin(); i != s.end(); ++i) {
            if (*i == '\\') {
                ++i;

                switch (*i) {
                case 'n': tmp += '\n'; break;
                case 'r': tmp += '\r'; break;
                case 't': tmp += '\t'; break;
                case '"': 
                case '\'': tmp += *i; break;
                default: {
                    unsigned char c = 0;
                    if (*i >= '0' && *i <= '9') { c += *i - '0'; }
                    else if (*i >= 'a' && *i <= 'f') { c += (*i - 'a') + 10; }
                    else if (*i >= 'A' && *i <= 'F') { c += (*i - 'A') + 10; }

                    c <<= 4;
                    ++i;
                    if (*i >= '0' && *i <= '9') { c += *i - '0'; }
                    else if (*i >= 'a' && *i <= 'f') { c += (*i - 'a') + 10; }
                    else if (*i >= 'A' && *i <= 'F') { c += (*i - 'A') + 10; }
                    tmp += c;
                }
                }
            } else {
                tmp += *i;
            }
        }

        t.push_back(Token(tmp));
    }
};


}

#endif
