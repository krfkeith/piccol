#ifndef __CROMLECH_ACTIONS_H
#define __CROMLECH_ACTIONS_H

#include <stdlib.h>

#include <vector>
#include <memory>
#include <unordered_map>

#include <pegtl.hh>

#include <iostream>


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

    std::string current_ns;

    std::unordered_map<Symbol, Val> typemap;

    std::vector<unsigned char> op;
    std::vector< std::pair<int,Symbol> > fun;

    Context() {} 

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

        //std::cout << "!" << tmp << std::endl;
        t.push_back(Opcall(LITERAL, tmp));
    }
};


struct a_struct_start : action_base<a_struct_start> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(LITERAL, empty_struct()));
    }
};

struct a_struct_end : action_base<a_struct_end> {
    static void apply(const std::string& s, Context& t) {}
};



struct a_struct_key : action_base<a_struct_key> {
    static void apply(const std::string& s, Context& t) {
        //std::cout<<"binding " << s<<std::endl;
        t.vm.code.back().val.binding = sym(s);
    }
};

struct a_struct_val : action_base<a_struct_val> {
    static void apply(const std::string& s, Context& t) {
	Val v;
        //std::cout<<"val swap " << s<<std::endl;
	v.swap(t.vm.code.back().val);
	t.vm.code.pop_back();
        //std::cout<<"val pushback " << s<<std::endl;
	get<Val::stup_t>(t.vm.code.back().val)->push_back(Val());
        //std::cout<<"val swap2 " << s<<std::endl;
	get<Val::stup_t>(t.vm.code.back().val)->back().swap(v);
        //std::cout<<"val ok " << s<<std::endl;
    }
};



struct a_type : action_base< a_type > {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(LITERAL, sym(s)));
	t.vm.code.back().val.type = Val::TYPETAG;
    }
};


struct a_custom_type : action_base< a_custom_type > {
    static void apply(const std::string& s, Context& t) {

	Symbol typ = sym(t.current_ns + "::" + s);

	auto tmp = t.typemap.find(typ);

	if (tmp == t.typemap.end()) {
	    throw std::runtime_error("Undefined type: " + t.current_ns + "::" + s);
	}

        t.vm.code.push_back(Opcall(LITERAL, tmp->second));
    }
};


struct a_varget : action_base<a_varget> {
    static void apply(const std::string& s, Context& t) {

        t.push_back(Opcall(GET, sym(s)));
    }
};


struct a_fun_struct_s : action_base<a_fun_struct_s> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(START_STRUCT, (Symbol)0));
    }
};

struct a_fun_struct_v : action_base<a_fun_struct_v> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(PUSH_STRUCT, (Symbol)0));
    }
};

struct a_frameget : action_base<a_frameget> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(FRAME_GET, (Symbol)0));
    }
};

struct a_fun_end : action_base<a_fun_end> {
    static void apply(const std::string& s, Context& t) {
        t.push_back(Opcall(RETURN, (Symbol)0));
    }
};

struct a_do_funcall : action_base<a_do_funcall> {
    static void apply(const std::string& s, Context& t) {
        auto v = t.fun.back();
        t.fun.pop_back();

        if (v.first) {
            t.push_back(Opcall(SYSCALL, v.second));
        } else {
            t.push_back(Opcall(FUNCALL, v.second));
        }
    }
};


struct a_setfun : action_base<a_setfun> {
    static void apply(const std::string& s, Context& t) {
        Symbol ss;
        ss = sym(t.current_ns + "::" + s);
        t.fun.push_back(std::make_pair(0, ss));
    }
};


struct a_setfun_ns : action_base<a_setfun_ns> {
    static void apply(const std::string& s, Context& t) {
        t.fun.push_back(std::make_pair(0, sym(s)));
    }
};

struct a_setsyscall : action_base<a_setsyscall> {
    static void apply(const std::string& s, Context& t) {
        t.fun.push_back(std::make_pair(1, sym(s)));
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
	t.current_ns = s;
    }
};

struct a_define_type : action_base<a_define_type> {
    static void apply(const std::string& s, Context& t) {

	t.typemap[sym(t.current_ns + "::" + s)] = t.vm.code.back().val;
	t.vm.code.pop_back();
    }
};


}

#endif
