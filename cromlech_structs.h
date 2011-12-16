#ifndef __CROMLECH_STRUCTS_H
#define __CROMLECH_STRUCTS_H

#include <string>
#include <unordered_map>
#include <vector>

namespace crom {

typedef long Int;

typedef double Real;

typedef size_t Symbol;

typedef bool Bool;

typedef std::string String;
    
struct SymTable {
    std::unordered_map<String,Symbol> m_string_to_symbol;
    std::unordered_map<Symbol,String> m_symbol_to_string;

    String get(Symbol s) {
        auto i = m_symbol_to_string.find(s);
        if (i == m_symbol_to_string.end())
            return "";
        return i->second;
    }

    Symbol get(const String& s) {
        auto i = m_string_to_symbol.find(s);
        if (i == m_string_to_symbol.end())
            return 0;
        return i->second;
    }

    Symbol add(const String& s) {
        auto i = m_string_to_symbol.find(s);

        if (i == m_string_to_symbol.end()) {
            Symbol ret = m_string_to_symbol.size() + 1;
            m_string_to_symbol[s] = ret;
            m_symbol_to_string[ret] = s;
            return ret;
        }

        return i->second;
    }
};


template <typename T>
T& Singleton() {
    static T t;
    return t;
}


inline Symbol sym(const String& s) {
    return Singleton<SymTable>().add(s);
}


inline Symbol _int() {
    static Symbol ret = sym("Int");
    return ret;
}

inline Symbol _real() {
    static Symbol ret = sym("Real");
    return ret;
}

inline Symbol _symbol() {
    static Symbol ret = sym("Symbol");
    return ret;
}

inline Symbol _bool() {
    static Symbol ret = sym("Bool");
    return ret;
}

inline Symbol _string() {
    static Symbol ret = sym("String");
    return ret;
}


struct PODLiteral {
    Symbol type;

    /* NOTE: This should be an anonymous union without constructors and destructors.
     * However, gcc 4.6.2 doesn't yet properly support unrestricted C++11 unions,
     * so I had to give the union a name and some dummy constructors/destructors.
     */

    union _d {
        Int i;
        Real r;
        Symbol s;
        Bool b;
        String str;

        _d() {}
        ~_d() {}
    } d;

    PODLiteral() { type = _int(); d.i = 0; }

    PODLiteral(Int _i)    { type = _int();    d.i = _i; }
    PODLiteral(Real _r)   { type = _real();   d.r = _r; }
    PODLiteral(Symbol _s) { type = _symbol(); d.s = _s; }
    PODLiteral(Bool _b)   { type = _bool();   d.b = _b; }

    PODLiteral(const String& _s) {
        type = _string();
        new (&d.str) String(_s);
    }

    ~PODLiteral() {
        if (type == _string()) {
            d.str.~String();
        }
    }

    PODLiteral(const PODLiteral& o) {
        if      (o.type == _int())    { d.i = o.d.i; }
        else if (o.type == _real())   { d.r = o.d.r; }
        else if (o.type == _symbol()) { d.s = o.d.s; }
        else if (o.type == _bool())   { d.b = o.d.b; }
        else if (o.type == _string()) {
            new (&d.str) String(o.d.str);
        }
        type = o.type;
    }

    PODLiteral& operator=(const PODLiteral& o) {
        if (this == &o) return *this;

        if (type == _string()) {
            d.str.~String();
        }

        if      (o.type == _int())    { d.i = o.d.i; }
        else if (o.type == _real())   { d.r = o.d.r; }
        else if (o.type == _symbol()) { d.s = o.d.s; }
        else if (o.type == _bool())   { d.b = o.d.b; }
        else if (o.type == _string()) {
            new (&d.str) String(o.d.str);
        }

        type = o.type;
        return *this;
    }

    template <typename F>
    void operator()(F&& f) const {
        if      (type == _int())    { f(d.i); }
        else if (type == _real())   { f(d.r); }
        else if (type == _symbol()) { f(d.s); }
        else if (type == _bool())   { f(d.b); }
        else if (type == _string()) { f(d.str); }
    }
};



}


#endif

