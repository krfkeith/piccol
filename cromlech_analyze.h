#ifndef __CROMLECH_ANALYZE_H
#define __CROMLECH_ANALYZE_H

#include <sstream>

namespace crom {

std::string symstr(Symbol s) {
    return crom::Singleton<crom::SymTable>().get(s);
}


struct _vprinter {

    std::ostringstream& os;
    _vprinter(std::ostringstream& o) : os(o) {}

    void operator()(const crom::Int& t) { os << t; }
    void operator()(const crom::Real& t) { os << t; }
    void operator()(const crom::Bool& t) { os << (t ? "\\true" : "\\false"); }
    void operator()(const crom::Symbol& t) { os << symstr(t); }
    void operator()(const crom::String& t) { os << "'" << t << "'"; }
    void operator()(const crom::Val::stup_t& t) {}
};


std::string print_type(const Val& v) {
    std::ostringstream os;

    if (v.type == Val::STRUCT) {

        os << "{ ";
        const Val::stup_t& vv = get<Val::stup_t>(v);
        for (const auto& t : *vv) {
            os << print_type(t) << " ";
        }
        os << "}";

    } else {
        v(_vprinter(os));
    }
    return os.str();
}


inline Val _check_type_typetag(const Val& type, Symbol funname, const Vm& vm) {

    if (type.type != Val::TYPETAG) {
        throw std::runtime_error("In function " + 
                                 symstr(funname) +
                                 ": a typename must be specified.");
    }

    Symbol s = get<Symbol>(type);

    if (s == _int() || s == _real() || s == _symbol() || 
        s == _bool() || s == _string()) {

        return type;
        //tis.push_back(type);

    } else {
        const auto i = vm.typemap.find(s);

        if (i == vm.typemap.end()) {
            throw std::runtime_error("In function " +
                                     symstr(funname) + 
                                     ": unknown type " + symstr(s));
        }

        return i->second;
        //tis.push_back(i->second);
    }
}

inline void check_type_typetag(const Val& type, Symbol funname, std::vector<Val>& tis, 
                               const Vm& vm) {

    tis.push_back(_check_type_typetag(type, funname, vm));
}

inline void check_type_literal(const Val& op, Symbol funname, std::vector<Val>& tis,
                               const Vm& vm) {
    switch (op.type) {
    case Val::INT:
        tis.push_back(_int());
        tis.back().type = Val::TYPETAG;
        break;
    case Val::REAL:
        tis.push_back(_real());
        tis.back().type = Val::TYPETAG;
        break;
    case Val::SYMBOL:
        tis.push_back(_symbol());
        tis.back().type = Val::TYPETAG;
        break;
    case Val::BOOL:
        tis.push_back(_bool());
        tis.back().type = Val::TYPETAG;
        break;
    case Val::STRING:
        tis.push_back(_string());
        tis.back().type = Val::TYPETAG;
        break;
    case Val::TYPETAG:
        check_type_typetag(op, funname, tis, vm);
        break;
    default:
        throw std::runtime_error("Sanity error: check_type_literal");
    }
}


inline void check_type_deref(Symbol field, int& arg, Symbol funname, 
                             std::vector<Val>& tis, const Vm& vm) {

    Val v = tis.back();
    tis.pop_back();

    if (v.type != Val::STRUCT) {
        throw std::runtime_error("In function " +
                                 symstr(funname) + 
                                 ": only structs support the '.' operator.");
    }

    const Val::stup_t& vv = get<Val::stup_t>(v);

    arg = 0;
    for (const auto& t : *vv) {
        if (t.binding == field) {
            check_type_typetag(t, funname, tis, vm);
            return;            
        }
        ++arg;
    }

    throw std::runtime_error("In function " +
                             symstr(funname) + 
                             ": no such struct field: " + symstr(field));
}

inline void check_type_numop(int& arg, Symbol funname, std::vector<Val>& tis) {

    Val v2 = tis.back();
    tis.pop_back();
    Val v1 = tis.back();
    tis.pop_back();

    if (v1.type != Val::TYPETAG || v2.type != Val::TYPETAG) {

        throw std::runtime_error("In function " +
                                 symstr(funname) + 
                                 ": operands to numeric ops must be atoms.");
    }

    Symbol _v1 = get<Symbol>(v1);
    Symbol _v2 = get<Symbol>(v2);

    if ((_v1 != _int() && _v1 != _real()) ||
        (_v2 != _int() && _v2 != _real())) {

        throw std::runtime_error("In function " +
                                 symstr(funname) + 
                                 ": operands to numeric ops must be numbers.");
    }

    if (_v1 == _real() || _v2 == _real()) {
        tis.push_back(_real());

        if (_v1 == _real() && _v2 == _real()) {
            arg = 1;
        } else if (_v1 == _real()) {
            arg = 2;
        } else {
            arg = 3;
        }

    } else {
        tis.push_back(_int());
        arg = 4;
    }

    tis.back().type = Val::TYPETAG;
}


inline bool check_type_equal(const Val& v, const Val& y, Symbol funname, const Vm& vm) {

    std::cout << "| " << v.type << " " << y.type << std::endl;

    if (v.type != y.type) {
        return false;
    }

    if (v.type == Val::TYPETAG && y.type == Val::TYPETAG) {
        Symbol vs = get<Symbol>(v);
        Symbol ys = get<Symbol>(y);

        std::cout << "| " << symstr(vs) << " " << symstr(ys) << std::endl;

        if (vs != ys) {
            return false;

        } else {
            std::cout << "True" << std::endl;
            return true;
        }

    } else if (v.type == Val::STRUCT && y.type == Val::STRUCT) {

        const Val::stup_t& vs = get<Val::stup_t>(v);
        const Val::stup_t& ys = get<Val::stup_t>(y);

        std::cout << "sz " << vs->size() << " " << ys->size() << std::endl;
        if (vs->size() != ys->size()) {
            return false;
        }

        for (int i = 0; i < vs->size(); ++i) {

            const Val& vsi = (*vs)[i];
            const Val& ysi = (*ys)[i];

            std::cout << " -- " << vsi.type << " " << ysi.type << std::endl;

            if (vsi.type != ysi.type) {
                return false;
            }

            if (vsi.type != Val::TYPETAG)
                return false;

            if (get<Symbol>(vsi) != get<Symbol>(ysi)) {
                return false;
            }

            /*
            Val yconv = _check_type_typetag((*ys)[i], funname, vm);

            if (!check_type_equal((*vs)[i], yconv, funname, vm)) {
                return false;
            }*/

        }
    }
    
    return true;
}

inline void check_type_funcall(Symbol calledfun, int& arg, Symbol funname, 
                               std::vector<Val>& tis, const Vm& vm) {

    Val v = tis.back();
    tis.pop_back();

    const auto tmp = vm.funs.equal_range(calledfun);

    std::cout << "//" << symstr(calledfun) << std::endl;

    arg = 0;
    for (auto i = tmp.first; i != tmp.second; ++i) {

        std::cout << "??" << std::endl;
        Val y = _check_type_typetag(i->second.type_from, funname, vm);

        std::cout << "Checking..." << std::endl;
        if (check_type_equal(v, y, funname, vm)) {

            std::cout << "OK, good." << std::endl;
            check_type_typetag(i->second.type_to, funname, tis, vm);
            return;
        }
        ++arg;
        std::cout << "Fail." << std::endl;
    }

    throw std::runtime_error("In function " +
                             symstr(funname) + 
                             ": could not find match for '" + symstr(calledfun) +
                             "$' " + print_type(v));
}




inline void check_type_syscall(Symbol calledfun, Symbol funname, 
                               std::vector<Val>& tis, const Vm& vm) {

    Val v = tis.back();
    tis.pop_back();

    const auto tmp = vm.syscalls.find(calledfun);

    if (tmp == vm.syscalls.end()) {
        throw std::runtime_error("In function " +
                                 symstr(funname) + 
                                 ": could not find match for '*" + symstr(calledfun) +
                                 "$'");
    }

    Val y = _check_type_typetag(tmp->second.type_from, funname, vm);

    if (check_type_equal(v, y, funname, vm)) {

        check_type_typetag(tmp->second.type_to, funname, tis, vm);
        return;
    }

    throw std::runtime_error("In function " +
                             symstr(funname) + 
                             ": type mismatch in calling '" + symstr(calledfun) +
                             "$':\n  [" + print_type(v) + " " + print_type(y) + "]");
}


inline void check_type_return(const Val& type, Symbol funname, std::vector<Val>& tis,
                              const Vm& vm) {

    Val v = tis.back();
    tis.pop_back();

    Val y = _check_type_typetag(type, funname, vm);

    std::cout << "Ret check..." << std::endl;
    if (!check_type_equal(v, y, funname, vm)) {

        throw std::runtime_error("In function " +
                                 symstr(funname) + 
                                 ": declared and actual return type mismatch:\n  [" +
                                 print_type(y) + " " + print_type(v) + "]");
    }

    if (tis.size() > 0) {
        throw std::runtime_error("In function " +
                                 symstr(funname) + 
                                 ": sanity error: values left on stack on return.");
    }
}


inline void check_type_op(Opcall& op, Symbol funname, std::vector<Val>& tis,
                          const Vm::fun& fun, const Vm& vm) {
    switch (op.type) {
    case LITERAL:
        check_type_literal(op.val, funname, tis, vm);
        break;

    case FRAME_GET:
        check_type_typetag(fun.type_from, funname, tis, vm);
        break;

    case GET:
        check_type_deref(get<Symbol>(op.val), op.arg, funname, tis, vm);
        break;

    case START_STRUCT:
        tis.push_back(empty_struct());
        break;

    case PUSH_STRUCT:
    {
        Val v;
        v.swap(tis.back());
        tis.pop_back();

        get<Val::stup_t>(tis.back())->push_back(Val());
        get<Val::stup_t>(tis.back())->back().swap(v);
        break;
    }

    case FUNCALL:
        check_type_funcall(get<Symbol>(op.val), op.arg, funname, tis, vm);
        break;

    case SYSCALL:
        check_type_syscall(get<Symbol>(op.val), funname, tis, vm);
        break;

    case RETURN:
        check_type_return(fun.type_to, funname, tis, vm);
        break;

    case CLEAR:
        tis.clear();
        break;

    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case MOD:
        check_type_numop(op.arg, funname, tis);
        break;
    }
}

inline void check_types(Symbol funname, Vm::fun& fun, const Vm& vm) {

    std::vector<Val> tinf_stack;

    for (auto& o : fun.code) {
        check_type_op(o, funname, tinf_stack, fun, vm);
    }

    //check_type(funname, fun.type_from, vm, tinf_stack);
    //check_type(funname, fun.type_to, vm);

}


inline void check_types(Vm& vm) {

    for (const auto& v : vm.funs) {
        std::cout << "{" << symstr(v.first) << std::endl;
    }

    for (auto& v : vm.funs) {
        check_types(v.first, v.second, vm);
    }
}



}

#endif
