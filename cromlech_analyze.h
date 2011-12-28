#ifndef __CROMLECH_ANALYZE_H
#define __CROMLECH_ANALYZE_H

namespace crom {

std::string symstr(Symbol s) {
    return crom::Singleton<crom::SymTable>().get(s);
}

inline void check_type_typetag(const Val& type, Symbol funname, std::vector<Val>& tis, 
                               const Vm& vm) {

    if (type.type != Val::TYPETAG) {
        throw std::runtime_error("In function " + 
                                 symstr(funname) +
                                 ": definition header must specify a typename.");
    }

    Symbol s = get<Symbol>(type);

    if (s == _int() || s == _real() || s == _symbol() || 
        s == _bool() || s == _string()) {

        tis.push_back(s);

    } else {
        const auto i = vm.typemap.find(s);

        if (i == vm.typemap.end()) {
            throw std::runtime_error("In function " +
                                     symstr(funname) + 
                                     ": unknown type " + symstr(s));
        }

        tis.push_back(i->second);
    }
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

inline void check_type_numop(Symbol funname, std::vector<Val>& tis) {

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
    } else {
        tis.push_back(_int());
    }
}

inline void check_type_op(const Opcall& op, Symbol funname, std::vector<Val>& tis,
                          const Vm::fun& fun, const Vm& vm) {
    switch (op.type) {
    case LITERAL:
        check_type_literal(op.val, funname, tis, vm);
        break;

    case FRAME_GET:
        check_type_typetag(fun.type_from, funname, tis, vm);
        break;

    case GET:
        throw std::runtime_error("TODO: GET");
        break;

    case START_STRUCT:
    case PUSH_STRUCT:
    case FUNCALL:
    case SYSCALL:

    case RETURN:
        break;

    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case MOD:
        check_type_numop(funname, tis);
        break;
    }
}

inline void check_types(Symbol funname, const Vm::fun& fun, const Vm& vm) {

    std::vector<Val> tinf_stack;

    for (const auto& o : fun.code) {
        check_type_op(o, funname, tinf_stack, fun, vm);
    }

    //check_type(funname, fun.type_from, vm, tinf_stack);
    //check_type(funname, fun.type_to, vm);

}


inline void check_types(const Vm& vm) {

    for (const auto& v : vm.funs) {
        check_types(v.first, v.second, vm);
    }
}



}

#endif
