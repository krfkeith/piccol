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



struct Context {

    Vm vm;

    Symbol current_ns;

    std::unordered_map<std::pair<Symbol,Symbol>, Val> typemap;
    std::map<Symbol, size_t> registermap;

    std::vector<unsigned char> op;
    std::vector<std::pair<int,Symbol>> fun;

    void push_back(const Opcall& t) {
        vm.code.push_back(t);
    }
};

struct a_int_literal : action_base<a_int_literal> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(LITERAL, ::atol(s.c_str())));
    }
};

struct a_real_literal : action_base<a_real_literal> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(LITERAL, ::atof(s.c_str())));
    }
};

struct a_symbol_literal : action_base<a_symbol_literal> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(LITERAL, sym(s)));
    }
};

struct a_bool_literal : action_base<a_bool_literal> {
    static void apply(const std::string& s, Context& t) {
        if (s == "\\true") {
            t.push_back(Opcall(LITERAL, true));
        } else {
            t.push_back(Opcall(LITERAL, false));
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

        t.push_back(Opcall(LITERAL, tmp));
    }
};


struct a_tuple_start : action_base<a_tuple_start> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(LITERAL, empty_tuple()));
    }
};

struct a_tuple_end : action_base<a_tuple_end> {
    static void apply(const std::string& s, Context& t) {}
};


struct a_struct_start : action_base<a_struct_start> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(LITERAL, empty_struct()));
    }
};

struct a_struct_end : action_base<a_struct_end> {
    static void apply(const std::string& s, Context& t) {}
};


struct a_tuple_element : action_base<a_tuple_element> {
    static void apply(const std::string& s, Context& t) {
	Val v;
	v.swap(t.vm.code.back().val);
	t.vm.code.pop_back();
	get<Val::stup_t>(t.vm.code.back().val)->push_back(Val());
	get<Val::stup_t>(t.vm.code.back().val)->back().swap(v);
    }
};

struct a_tuple_clone : action_base<a_tuple_clone> {
    static void apply(const std::string& s, Context& t) {

	Val& tup = t.vm.code.back().val;
	Val& tupe = get<Val::stup_t>(tup)->back();

        int num = ::atol(s.c_str());
	for (int i = 1; i < num; ++i) {
	    get<Val::stup_t>(tup)->push_back(tupe);
	}
    }
};

struct a_struct_key : action_base<a_struct_key> {
    static void apply(const std::string& s, Context& t) {
	Val& stru = t.vm.code.back().val;
	get<Val::stup_t>(stru)->push_back(sym(s));
    }
};

struct a_struct_val : action_base<a_struct_val> {
    static void apply(const std::string& s, Context& t) {
	Val v;
	v.swap(t.vm.code.back().val);
	t.vm.code.pop_back();
	get<Val::stup_t>(t.vm.code.back().val)->push_back(Val());
	get<Val::stup_t>(t.vm.code.back().val)->back().swap(v);
    }
};



struct a_type : action_base< a_type > {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(TYPE, sym(s)));
	t.vm.code.back().val.type = Val::TYPETAG;
    }
};


struct a_custom_type : action_base< a_custom_type > {
    static void apply(const std::string& s, Context& t) {

	Symbol typ = sym(s);

	auto tmp = t.typemap.find(std::make_pair(t.current_ns, typ));

	if (tmp == t.typemap.end()) {
	    throw std::runtime_error("Undefined type: " + s);
	}

        t.vm.code.push_back(Opcall(LITERAL, tmp->second));
    }
};



struct a_vardef : action_base<a_vardef> {
    static void apply(const std::string& s, Context& t) {
        Symbol var = sym(s);

        auto i = t.registermap.find(var);

        if (i == t.registermap.end()) {
            i = t.registermap.insert(std::make_pair(var, t.registermap.size()+1)).first;
        }

        t.vm.code.back().val.binding = i->second;
    }
};

struct a_varget : action_base<a_varget> {
    static void apply(const std::string& s, Context& t) {
        Symbol var = sym(s);

        auto i = t.registermap.find(var);

        if (i == t.registermap.end())
            throw std::runtime_error("Reference to an undefined variable: " + s);

        t.push_back(Opcall(BIND, (Symbol)0));
        t.vm.code.back().val.type = Val::PLACEHOLDER;
        t.vm.code.back().val.binding = i->second;
    }
};


struct a_do_match : action_base<a_do_match> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(BIND, (Symbol)0));
        t.vm.code.back().val.type = Val::PLACEHOLDER;
        t.vm.code.back().val.binding = 0;
        t.push_back(Opcall(IFMATCH, (Int)2));
        t.push_back(Opcall(RETURN, (Symbol)0));
    }
};

struct a_fun_end : action_base<a_do_match> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(RETURN, (Symbol)0));
    }
};

struct a_do_funcall : action_base<a_do_funcall> {
    static void apply(const std::string& s, Context& t) {
        std::pair<int,Symbol> v = t.fun.back();
        t.fun.pop_back();
        t.push_back(Opcall(FUNCALL, v.second));
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
            t.push_back(Opcall(ADD, (Symbol)0));
            break;
        case '-':
            t.push_back(Opcall(SUB, (Symbol)0));
            break;
        case '*':
            t.push_back(Opcall(MUL, (Symbol)0));
            break;
        case '/':
            t.push_back(Opcall(DIV, (Symbol)0));
            break;
        case '%':
            t.push_back(Opcall(MOD, (Symbol)0));
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
	    t.vm.code.back().val;
	t.vm.code.pop_back();
    }
};


}

#endif
