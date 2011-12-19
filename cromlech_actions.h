#ifndef __CROMLECH_ACTIONS_H
#define __CROMLECH_ACTIONS_H

#include <stdlib.h>

#include <vector>
#include <memory>
#include <unordered_map>

#include <pegtl.hh>


namespace std {

template <typename A, typename B>
struct hash< pair<A,B> > {
    size_t operator()(const pair<A,B>& p) const {
	return hash<A>()(p.first) ^ hash<B>()(p.second);
    }
};

}

namespace crom {

using namespace pegtl;

enum {
    LITERAL = 1,
    TYPE,
    VARIABLE,

    IFMATCH,
    RETURN,
    FUNCALL,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
};

struct Token {
    int type;
    Val val;
    
    Token() {}
    
    template <typename T>
    Token(int tp, const T& t) : type(tp), val(t) {}
};


struct Context {

    typedef std::vector< Token > stack_t;

    Symbol current_ns;

    stack_t stack;

    std::unordered_map<std::pair<Symbol,Symbol>,
		       stack_t::value_type> typemap;

    std::map<Symbol, size_t> registermap;

    std::vector<unsigned char> op;
    std::vector<std::pair<int,Symbol>> fun;

    void push_back(const Token& t) {
        stack.push_back(t);
    }

    auto begin() const -> decltype(stack.cbegin()) { return stack.begin(); }
    auto end() const -> decltype(stack.cend()) { return stack.end(); }
};

struct a_int_literal : action_base<a_int_literal> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Token(LITERAL, ::atol(s.c_str())));
    }
};

struct a_real_literal : action_base<a_real_literal> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Token(LITERAL, ::atof(s.c_str())));
    }
};

struct a_symbol_literal : action_base<a_symbol_literal> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Token(LITERAL, sym(s)));
    }
};

struct a_bool_literal : action_base<a_bool_literal> {
    static void apply(const std::string& s, Context& t) {
        if (s == "\\true") {
            t.push_back(Token(LITERAL, true));
        } else {
            t.push_back(Token(LITERAL, false));
        }
    }
};

struct a_string_literal : action_base<a_string_literal> {
    static void apply(const std::string& s, Context& t) {
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
    static void apply(const std::string& s, Context& t) {
        t.push_back(Token(LITERAL, empty_tuple()));
    }
};

struct a_tuple_end : action_base<a_tuple_end> {
    static void apply(const std::string& s, Context& t) {}
};


struct a_struct_start : action_base<a_struct_start> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Token(LITERAL, empty_struct()));
    }
};

struct a_struct_end : action_base<a_struct_end> {
    static void apply(const std::string& s, Context& t) {}
};


struct a_tuple_element : action_base<a_tuple_element> {
    static void apply(const std::string& s, Context& t) {
	Val v;
	v.swap(t.stack.back().val);
	t.stack.pop_back();
	get<Val::stup_t>(t.stack.back().val)->push_back(Val());
	get<Val::stup_t>(t.stack.back().val)->back().swap(v);
    }
};

struct a_tuple_clone : action_base<a_tuple_clone> {
    static void apply(const std::string& s, Context& t) {

	Val& tup = t.stack.back().val;
	Val& tupe = get<Val::stup_t>(tup)->back();

        int num = ::atol(s.c_str());
	for (int i = 1; i < num; ++i) {
	    get<Val::stup_t>(tup)->push_back(tupe);
	}
    }
};

struct a_struct_key : action_base<a_struct_key> {
    static void apply(const std::string& s, Context& t) {
	Val& stru = t.stack.back().val;
	get<Val::stup_t>(stru)->push_back(sym(s));
    }
};

struct a_struct_val : action_base<a_struct_val> {
    static void apply(const std::string& s, Context& t) {
	Val v;
	v.swap(t.stack.back().val);
	t.stack.pop_back();
	get<Val::stup_t>(t.stack.back().val)->push_back(Val());
	get<Val::stup_t>(t.stack.back().val)->back().swap(v);
    }
};



struct a_type : action_base< a_type > {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Token(TYPE, sym(s)));
	t.stack.back().val.type = Val::TYPETAG;
    }
};


struct a_custom_type : action_base< a_custom_type > {
    static void apply(const std::string& s, Context& t) {

	Symbol typ = sym(s);

	auto tmp = t.typemap.find(std::make_pair(t.current_ns, typ));

	if (tmp == t.typemap.end()) {
	    throw std::runtime_error("Undefined type: " + s);
	}

        t.stack.push_back(tmp->second);
    }
};



struct a_vardef : action_base<a_vardef> {
    static void apply(const std::string& s, Context& t) {
        Symbol var = sym(s);

        auto i = t.registermap.find(var);

        if (i == t.registermap.end()) {
            i = t.registermap.insert(std::make_pair(var, t.registermap.size()+1)).first;
        }

        t.stack.back().val.binding = i->second;
    }
};

struct a_varget : action_base<a_varget> {
    static void apply(const std::string& s, Context& t) {
        Symbol var = sym(s);

        auto i = t.registermap.find(var);

        if (i == t.registermap.end())
            throw std::runtime_error("Reference to an undefined variable: " + s);

        t.push_back(Token(VARIABLE, (Symbol)0));
        t.stack.back().val.type = Val::PLACEHOLDER;
        t.stack.back().val.binding = i->second;
    }
};


struct a_do_match : action_base<a_do_match> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Token(IFMATCH, (Int)1234));
    }
};

struct a_fun_end : action_base<a_do_match> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Token(RETURN, (Symbol)0));
    }
};

struct a_do_funcall : action_base<a_do_funcall> {
    static void apply(const std::string& s, Context& t) {
        std::pair<int,Symbol> v = t.fun.back();
        t.fun.pop_back();
        t.push_back(Token(FUNCALL, v.second));
    }
};

struct a_setfun : action_base<a_setfun> {
    static void apply(const std::string& s, Context& t) {
        if (s[0] == '*') {
            t.fun.push_back(std::make_pair(1, sym(s.substr(1))));
        } else {
            t.fun.push_back(std::make_pair(0, sym(s)));
        }
    }
};

struct a_setop : action_base<a_setop> {
    static void apply(const std::string& s, Context& t) {
        t.op.push_back(s[0]);
    }
};

struct a_do_op : action_base<a_do_op> {
    static void apply(const std::string& s, Context& t) {
        unsigned char op = t.op.back();
        t.op.pop_back();

        switch (op) {
        case '+':
            t.push_back(Token(ADD, (Symbol)0));
            break;
        case '-':
            t.push_back(Token(SUB, (Symbol)0));
            break;
        case '*':
            t.push_back(Token(MUL, (Symbol)0));
            break;
        case '/':
            t.push_back(Token(DIV, (Symbol)0));
            break;
        case '%':
            t.push_back(Token(MOD, (Symbol)0));
            break;
        }
    }
};


struct a_set_namespace : action_base<a_set_namespace> {
    static void apply(const std::string& s, Context& t) {
	t.current_ns = sym(s);
    }
};

struct a_define_type : action_base<a_define_type> {
    static void apply(const std::string& s, Context& t) {

	t.typemap[std::make_pair(t.current_ns, sym(s))] = 
	    t.stack.back();
	t.stack.pop_back();
    }
};


}

#endif
