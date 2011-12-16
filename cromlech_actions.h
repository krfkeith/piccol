#ifndef __CROMLECH_ACTIONS_H
#define __CROMLECH_ACTIONS_H

#include <stdlib.h>

#include <vector>
#include <memory>

#include <pegtl.hh>


namespace crom {

using namespace pegtl;

enum {
    LITERAL = 1,
    TUPLE_ELEMENT,
    TUPLE_CLONE,
    STRUCT_KEY,
    STRUCT_VAL,

    TUPLE_LITERAL,
    STRUCT_LITERAL,

    SYMBOL_TYPE,
    INT_TYPE,
    REAL_TYPE,
    BOOL_TYPE,
    STRING_TYPE,
    CUSTOM_TYPE,
    TUPLE_TYPE,
    STRUCT_TYPE,

    MATCH_TUPLE,
    MATCH_STRUCT,

    VARDEF,
    VARGET,

    TUPLE_START,
    STRUCT_START,
    TUPLE_END,
    STRUCT_END
};

struct Token {
    int type;
    PODLiteral val;
    
    Token() {}
    
    template <typename T>
    Token(int tp, const T& t) : type(tp), val(t) {}
};

struct Node {
    Token token;
    std::vector< std::shared_ptr<Node> > edges;
};

struct stack_t {
    std::vector< std::shared_ptr<Node> > stack;

    void push_back(const Token& t) {
        std::shared_ptr<Node> n(new Node);
        n->token = t;
        stack.push_back(n);
    }

    void push_in_back(const Token& t) {
        std::shared_ptr<Node> n(new Node);
        n->token = t;
        stack.back()->edges.push_back(n);
    }

    void nudge() {
        std::shared_ptr<Node> n = stack.back();
        stack.pop_back();
        stack.back()->edges.push_back(n);
    }

    auto begin() const -> decltype(stack.cbegin()) { return stack.begin(); }
    auto end() const -> decltype(stack.cend()) { return stack.end(); }
};

struct a_int_literal : action_base<a_int_literal> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(LITERAL, ::atol(s.c_str())));
    }
};

struct a_real_literal : action_base<a_real_literal> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(LITERAL, ::atof(s.c_str())));
    }
};

struct a_symbol_literal : action_base<a_symbol_literal> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(LITERAL, sym(s)));
    }
};

struct a_bool_literal : action_base<a_bool_literal> {
    static void apply(const std::string& s, stack_t& t) {
        if (s == "\\true") {
            t.push_back(Token(LITERAL, true));
        } else {
            t.push_back(Token(LITERAL, false));
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

        t.push_back(Token(LITERAL, tmp));
    }
};


struct a_tuple_start : action_base<a_tuple_start> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(TUPLE_START, (Symbol)0));
    }
};

struct a_tuple_end : action_base<a_tuple_end> {
    static void apply(const std::string& s, stack_t& t) {
        //t.push_back(Token(TUPLE_END, (Symbol)0));
    }
};


struct a_struct_start : action_base<a_struct_start> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(STRUCT_START, (Symbol)0));
    }
};

struct a_struct_end : action_base<a_struct_end> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(STRUCT_END, (Symbol)0));
    }
};


struct a_tuple_element : action_base<a_tuple_element> {
    static void apply(const std::string& s, stack_t& t) {
        //t.push_back(Token(TUPLE_ELEMENT, (Symbol)0));
        t.nudge();
    }
};

struct a_tuple_clone : action_base<a_tuple_clone> {
    static void apply(const std::string& s, stack_t& t) {
        //t.push_back(Token(TUPLE_CLONE, ::atol(s.c_str())));

        std::shared_ptr<Node> n = t.stack.back();
        t.stack.pop_back();

        int num = ::atol(s.c_str());
        for (int i = 0; i < num; ++i) {
            t.stack.back()->edges.push_back(n);
        }
    }
};

struct a_struct_key : action_base<a_struct_key> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_in_back(Token(STRUCT_KEY, sym(s)));
    }
};

struct a_struct_val : action_base<a_struct_val> {
    static void apply(const std::string& s, stack_t& t) {
        t.nudge();
        //t.push_in_back(Token(STRUCT_VAL, (Symbol)0));
    }
};


struct a_tuple_literal : action_base<a_tuple_literal> {
    static void apply(const std::string& s, stack_t& t) {
        //t.push_back(Token(TUPLE_LITERAL, (Symbol)0));
    }
};

struct a_struct_literal : action_base<a_struct_literal> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(STRUCT_LITERAL, (Symbol)0));
    }
};


template <int TT>
struct a_type : action_base< a_type<TT> > {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(TT, (Symbol)0));
    }
};


struct a_match_tuple : action_base<a_match_tuple> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(MATCH_TUPLE, (Symbol)0));
    }
};

struct a_match_struct : action_base<a_match_struct> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(MATCH_STRUCT, (Symbol)0));
    }
};

struct a_vardef : action_base<a_vardef> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(VARDEF, sym(s)));
    }
};

struct a_varget : action_base<a_varget> {
    static void apply(const std::string& s, stack_t& t) {
        t.push_back(Token(VARGET, sym(s)));
    }
};


}

#endif
