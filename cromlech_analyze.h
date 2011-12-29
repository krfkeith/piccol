#ifndef __CROMLECH_ANALYZE_H
#define __CROMLECH_ANALYZE_H

namespace crom {

std::string symstr(Symbol s) {
    return crom::Singleton<crom::SymTable>().get(s);
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

        return s;
        //tis.push_back(s);

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
        break;
    case Val::REAL:
        tis.push_back(_real());
        break;
    case Val::SYMBOL:
        tis.push_back(_symbol());
        break;
    case Val::BOOL:
        tis.push_back(_bool());
        break;
    case Val::STRING:
        tis.push_back(_string());
        break;
    case Val::TYPETAG:
        check_type_typetag(op, funname, tis, vm);
        break;
    default:
        throw std::runtime_error("TODO: check_type_literal");
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

    if (v1.type != Val::SYMBOL || v2.type != Val::SYMBOL) {

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
}


inline bool check_type_equal(const Val& v, const Val& y, Symbol funname, const Vm& vm) {

    std::cout << "| " << v.type << " " << y.type << std::endl;

    if (v.type != y.type) {
        return false;
    }

    if (v.type == Val::SYMBOL && y.type == Val::SYMBOL) {
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

            Val yconv = _check_type_typetag((*ys)[i], funname, vm);

            if (!check_type_equal((*vs)[i], yconv, funname, vm)) {
                return false;
            }
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
                             "$'");
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
                                 ": declared and actual return type mismatch.");
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
        throw std::runtime_error("TODO");
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
