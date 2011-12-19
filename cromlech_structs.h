#ifndef __CROMLECH_STRUCTS_H
#define __CROMLECH_STRUCTS_H

#include <string.h>

#include <string>
#include <memory>
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


////

struct Val {
    enum {
	INT = 1,
	REAL = 2,
	SYMBOL = 3,
	BOOL = 4,
	STRING = 5,
	TUPLE = 6,
	STRUCT = 7,
	TYPETAG = 8
    };

    unsigned short type;
    unsigned short binding;

    /* NOTE: This should be an anonymous union without constructors and destructors.
     * However, gcc 4.6.2 doesn't yet properly support unrestricted C++11 unions,
     * so I had to give the union a name and some dummy constructors/destructors.
     */

    typedef std::shared_ptr< std::vector<Val> > stup_t;

    union _d {
        Int i;
        Real r;
        Symbol s;
        Bool b;
        String str;

	stup_t stup;

        _d() {}
        ~_d() {}
    } d;

private:

    void _destroy() {
	switch (type) {
	case STRING: 
	    d.str.~String(); 
	    break;
	case STRUCT:
	case TUPLE:
	    d.stup.~stup_t();
	    break;
        }
    }

    void _copy(const Val& o) {
	switch (o.type) {
	case INT:
	    d.i = o.d.i;
	    break;
	case REAL:
	    d.r = o.d.r;
	    break;
	case SYMBOL:
	case TYPETAG:
	    d.s = o.d.s;
	    break;
	case BOOL:
	    d.b = o.d.b;
	    break;
	case STRING:
            new (&d.str) String(o.d.str);
	    break;
	case STRUCT:
	case TUPLE:
	    new (&d.stup) stup_t(o.d.stup);
        }
        type = o.type;
        binding = o.binding;
    }

public:

    Val() : type(INT), binding(0) { d.i = 0; }

    Val(Int _i) : type(INT), binding(0)       { d.i = _i; }
    Val(Real _r) : type(REAL), binding(0)     { d.r = _r; }
    Val(Symbol _s) : type(SYMBOL), binding(0) { d.s = _s; }
    Val(Bool _b) : type(BOOL), binding(0)     { d.b = _b; }

    Val(const String& _s) : type(STRING), binding(0) {
        new (&d.str) String(_s);
    }

    ~Val() {
	_destroy();
    }

    Val(const Val& o) {
	_copy(o);
    }

    Val& operator=(const Val& o) {
        if (this == &o) return *this;

	_destroy();
	_copy(o);
        return *this;
    }

    void swap(Val& o) {
	unsigned short otype = o.type;
        unsigned short obinding = o.binding;

	char od[sizeof(_d)];
	::memcpy(&od[0], &o.d, sizeof(_d));

	::memcpy(&o.d, &d, sizeof(_d));
	::memcpy(&d, &od[0], sizeof(_d));

	o.type = type;
        o.binding = binding;
	type = otype;
        binding = obinding;
    }

    template <typename F>
    void operator()(F&& f) const {
	switch (type) {
	case INT:
	    f(d.i);
	    break;
	case REAL:
	    f(d.r);
	    break;
	case SYMBOL:
	case TYPETAG:
	    f(d.s);
	    break;
	case BOOL:
	    f(d.b);
	    break;
	case STRING:
	    f(d.str);
	    break;
	case STRUCT:
	case TUPLE:
	    f(d.stup);
	    break;
	}
    }
};

inline Val empty_tuple() {
    Val ret;
    new (&ret.d.stup) Val::stup_t(new std::vector<Val>());
    ret.type = Val::TUPLE;
    ret.binding = 0;
    return ret;
}

inline Val empty_struct() {
    Val ret;
    new (&ret.d.stup) Val::stup_t(new std::vector<Val>());
    ret.type = Val::STRUCT;
    ret.binding = 0;
    return ret;
}


template <typename T> T& get(Val&);

template <> Int& get(Val& v) { return v.d.i; }
template <> Real& get(Val& v) { return v.d.r; }
template <> Symbol& get(Val& v) { return v.d.s; }
template <> Bool& get(Val& v) { return v.d.b; }
template <> String& get(Val& v) { return v.d.str; }
template <> Val::stup_t& get(Val& v) { return v.d.stup; }





}


#endif

