#ifndef __NANOM_H
#define __NANOM_H

#include <ctype.h>

#include <cstdint>
#include <stdexcept>

#include <string>
#include <vector>
#include <unordered_map>

#include "metalan.h"


namespace std {
template <typename A, typename B>
struct hash< pair<A,B> > {
    size_t operator()(const pair<A,B>& p) const {
        return hash<A>()(p.first) ^ hash<B>()(p.second);
    }
};
}



namespace nanom {

using metalan::Sym;
using metalan::symtab;

typedef int64_t Int;
typedef uint64_t UInt;
typedef double Real;

union Val {
    Int inte;
    UInt uint;
    Real real;

    Val() : inte(0) {}
    Val(Int i)  { inte = i; }
    Val(UInt i) { uint = i; }
    Val(Real i) { real = i; }
};

enum Type {
    NONE   = 0,
    BOOL   = 1,
    SYMBOL = 2,
    INT    = 3, 
    UINT   = 4,
    REAL   = 5,
    STRUCT = 6
};


struct Shape {
    struct typeinfo {
        Type type;
        Sym shape;
        size_t ix_from;
        size_t ix_to;

        typeinfo() : type(NONE), shape(0), ix_from(1), ix_to(0) {}
    };

    std::unordered_map<Sym, typeinfo> sym2field;

    size_t nfields;

    Shape() : nfields(0) {}

    const typeinfo& get_type(Sym s) const {
        auto i = sym2field.find(s);

        if (i == sym2field.end()) {
            static typeinfo notype;
            return notype;
        }

        return i->second;
    }

    std::pair<size_t,size_t> get_index(Sym s) const {
        auto i = sym2field.find(s);
        if (i == sym2field.end())
            return std::make_pair(1,0);
        return std::make_pair(i->second.ix_from, i->second.ix_to);
    }

    void add_field(Sym s, Type t, Sym shape=0, size_t sh_size=0) {
        if (sym2field.count(s) != 0) {
            throw std::runtime_error("Cannot add duplicate field to shape.");
        }

        typeinfo& ti = sym2field[s];

        ti.type = t;
        ti.shape = shape;
        ti.ix_from = nfields;

        if (t == STRUCT) {
            nfields += sh_size;

        } else {
            ++nfields;
        }

        ti.ix_to = nfields;
    }

    const typeinfo& get_type(const std::string& s) const { 
        return get_type(symtab().get(s)); 
    }

    std::pair<size_t,size_t> get_index(const std::string& s) const { 
        return get_index(symtab().get(s)); 
    }

    size_t size() const { return nfields; }
};

struct Shapes {
    std::unordered_map<Sym,Shape> shapes;

    const Shape& get(Sym shapeid) const {
        auto i = shapes.find(shapeid);

        if (i == shapes.end())
            throw std::runtime_error("Unknown shape name: " + symtab().get(shapeid));

        return i->second;
    }

    void add(Sym shapeid, const Shape& sh) {

        auto i = shapes.find(shapeid);

        if (i != shapes.end())
            throw std::runtime_error("Cannot redefine shape: " + symtab().get(shapeid));

        shapes.insert(i, std::make_pair(shapeid, sh));
    }
};


struct Struct {
    typedef std::vector<Val> value_type;
    value_type v;

    Struct(size_t n = 0) {
        v.resize(n);
    }

    Struct(Struct&& s) {
        v.swap(s.v);
    }

    const Val& get_field(size_t i) const {
        return v.at(i);
    }

    void set_field(size_t i, Val val) {
        v[i] = val;
    }

    Struct substruct(size_t b, size_t i) const {
        Struct ret;
        ret.v.assign(v.begin() + b, v.begin() + i);
        return ret;
    }
};





enum op_t {
    NOOP = 0,

    PUSH,
    POP,
    SWAP,
    DUP,
    PUSH_DUP,

    CALL,
    EXIT,

    ADD_INT,
    SUB_INT,
    MUL_INT,
    DIV_INT,
    NEG_INT,

    ADD_UINT,
    SUB_UINT,
    MUL_UINT,
    DIV_UINT,

    ADD_REAL,
    SUB_REAL,
    MUL_REAL,
    DIV_REAL,
    NEG_REAL,

    BAND,
    BOR,
    BNOT,
    BXOR,
    BSHL,
    BSHR,

    EQ_INT,
    LT_INT,
    LTE_INT,
    GT_INT,
    GTE_INT,

    EQ_UINT,
    LT_UINT,
    LTE_UINT,
    GT_UINT,
    GTE_UINT,

    EQ_REAL,
    LT_REAL,
    LTE_REAL,
    GT_REAL,
    GTE_REAL,

    NEW_SHAPE,
    DEF_FIELD,
    DEF_STRUCT_FIELD,
    DEF_SHAPE,

    NEW_STRUCT,
    SET_FIELDS,

    SYSCALL_STRUCT,
    SYSCALL_PRIMITIVE

};


struct Opcode {
    op_t op;
    Val arg;
};

struct VmCode {
    typedef std::vector<Opcode> code_t;
    typedef std::pair<Sym,Sym> label_t;

    std::unordered_map<label_t, code_t> codes;

    static label_t toplevel_label() {
        Sym none = symtab().get("");
        return std::make_pair(none, none);
    }
};

typedef void (*syscall_callback_t)(const Shapes&, const Shape&, const Struct&);
typedef void (*sysfunc_callback_t)(const Shapes&, const Shape&, const Shape&,
                                   const Struct&, Struct&);


struct Vm {

    std::vector<Val> stack;
    std::vector< std::pair<VmCode::label_t, size_t> > frame;

    VmCode& code;

    Shapes shapes;

    std::unordered_map<Sym, syscall_callback_t> callbacks;
    std::unordered_map<VmCode::label_t, sysfunc_callback_t> func_callbacks;

    Shape tmp_shape;


    Vm(VmCode& c) : code(c) {}

    void register_callback(Sym s, syscall_callback_t cb) {
        callbacks[s] = cb;
    }

    void register_callback(VmCode::label_t s, sysfunc_callback_t cb) {
        func_callbacks[s] = cb;
    }

    Val pop() {
        Val ret = stack.back();
        stack.pop_back();
        return ret;
    }

    void push(Val v) {
        stack.push_back(v);
    }

};

namespace {


struct _mapper {
    std::unordered_map<size_t,std::string> m;
    std::unordered_map<std::string,op_t> n;
    _mapper() {
        m[(size_t)NOOP] = "NOOP";
        m[(size_t)PUSH] = "PUSH";
        m[(size_t)POP] = "POP";
        m[(size_t)SWAP] = "SWAP";
        m[(size_t)DUP] = "DUP";
        m[(size_t)PUSH_DUP] = "PUSH_DUP";
        m[(size_t)CALL] = "CALL";
        m[(size_t)EXIT] = "EXIT";
        m[(size_t)ADD_INT] = "ADD_INT";
        m[(size_t)SUB_INT] = "SUB_INT";
        m[(size_t)MUL_INT] = "MUL_INT";
        m[(size_t)DIV_INT] = "DIV_INT";
        m[(size_t)NEG_INT] = "NEG_INT";
        m[(size_t)ADD_UINT] = "ADD_UINT";
        m[(size_t)SUB_UINT] = "SUB_UINT";
        m[(size_t)MUL_UINT] = "MUL_UINT";
        m[(size_t)DIV_UINT] = "DIV_UINT";
        m[(size_t)ADD_REAL] = "ADD_REAL";
        m[(size_t)SUB_REAL] = "SUB_REAL";
        m[(size_t)MUL_REAL] = "MUL_REAL";
        m[(size_t)DIV_REAL] = "DIV_REAL";
        m[(size_t)NEG_REAL] = "NEG_REAL";
        m[(size_t)BAND] = "BAND";
        m[(size_t)BOR] = "BOR";
        m[(size_t)BNOT] = "BNOT";
        m[(size_t)BXOR] = "BXOR";
        m[(size_t)BSHL] = "BSHL";
        m[(size_t)BSHR] = "BSHR";
        m[(size_t)EQ_INT] = "EQ_INT";
        m[(size_t)LT_INT] = "LT_INT";
        m[(size_t)LTE_INT] = "LTE_INT";
        m[(size_t)GT_INT] = "GT_INT";
        m[(size_t)GTE_INT] = "GTE_INT";
        m[(size_t)EQ_UINT] = "EQ_UINT";
        m[(size_t)LT_UINT] = "LT_UINT";
        m[(size_t)LTE_UINT] = "LTE_UINT";
        m[(size_t)GT_UINT] = "GT_UINT";
        m[(size_t)GTE_UINT] = "GTE_UINT";
        m[(size_t)EQ_REAL] = "EQ_REAL";
        m[(size_t)LT_REAL] = "LT_REAL";
        m[(size_t)LTE_REAL] = "LTE_REAL";
        m[(size_t)GT_REAL] = "GT_REAL";
        m[(size_t)GTE_REAL] = "GTE_REAL";
        m[(size_t)NEW_SHAPE] = "NEW_SHAPE";
        m[(size_t)DEF_FIELD] = "DEF_FIELD";
        m[(size_t)DEF_STRUCT_FIELD] = "DEF_STRUCT_FIELD";
        m[(size_t)DEF_SHAPE] = "DEF_SHAPE";
        m[(size_t)NEW_STRUCT] = "NEW_STRUCT";
        m[(size_t)SET_FIELDS] = "SET_FIELDS";
        m[(size_t)SYSCALL_STRUCT] = "SYSCALL_STRUCT";
        m[(size_t)SYSCALL_PRIMITIVE] = "SYSCALL_PRIMITIVE";
        
        n["NOOP"] = NOOP;
        n["PUSH"] = PUSH;
        n["POP"] = POP;
        n["SWAP"] = SWAP;
        n["DUP"] = DUP;
        n["PUSH_DUP"] = PUSH_DUP;
        n["CALL"] = CALL;
        n["EXIT"] = EXIT;
        n["ADD_INT"] = ADD_INT;
        n["SUB_INT"] = SUB_INT;
        n["MUL_INT"] = MUL_INT;
        n["DIV_INT"] = DIV_INT;
        n["NEG_INT"] = NEG_INT;
        n["ADD_UINT"] = ADD_UINT;
        n["SUB_UINT"] = SUB_UINT;
        n["MUL_UINT"] = MUL_UINT;
        n["DIV_UINT"] = DIV_UINT;
        n["ADD_REAL"] = ADD_REAL;
        n["SUB_REAL"] = SUB_REAL;
        n["MUL_REAL"] = MUL_REAL;
        n["DIV_REAL"] = DIV_REAL;
        n["NEG_REAL"] = NEG_REAL;
        n["BAND"] = BAND;
        n["BOR"] = BOR;
        n["BNOT"] = BNOT;
        n["BXOR"] = BXOR;
        n["BSHL"] = BSHL;
        n["BSHR"] = BSHR;
        n["EQ_INT"] = EQ_INT;
        n["LT_INT"] = LT_INT;
        n["LTE_INT"] = LTE_INT;
        n["GT_INT"] = GT_INT;
        n["GTE_INT"] = GTE_INT;
        n["EQ_UINT"] = EQ_UINT;
        n["LT_UINT"] = LT_UINT;
        n["LTE_UINT"] = LTE_UINT;
        n["GT_UINT"] = GT_UINT;
        n["GTE_UINT"] = GTE_UINT;
        n["EQ_REAL"] = EQ_REAL;
        n["LT_REAL"] = LT_REAL;
        n["LTE_REAL"] = LTE_REAL;
        n["GT_REAL"] = GT_REAL;
        n["GTE_REAL"] = GTE_REAL;
        n["NEW_SHAPE"] = NEW_SHAPE;
        n["DEF_FIELD"] = DEF_FIELD;
        n["DEF_STRUCT_FIELD"] = DEF_STRUCT_FIELD;
        n["DEF_SHAPE"] = DEF_SHAPE;
        n["NEW_STRUCT"] = NEW_STRUCT;
        n["SET_FIELDS"] = SET_FIELDS;
        n["SYSCALL_STRUCT"] = SYSCALL_STRUCT;
        n["SYSCALL_PRIMITIVE"] = SYSCALL_PRIMITIVE;
    }
};


const std::string& opcodename(op_t opc) {
    static _mapper m;
    return m.m[opc];
}


const op_t opcodecode(const std::string& opc) {
    static _mapper m;
    return m.n[opc];
}

}


inline void vm_run(Vm& vm, 
                   VmCode::label_t label = VmCode::toplevel_label(), 
                   size_t ip = 0, 
                   bool verbose = false) {

    Sym int_primitive = symtab().get("Int");
    Sym real_primitive = symtab().get("Real");
    Sym uint_primitive = symtab().get("UInt");
    Sym bool_primitive = symtab().get("Bool");
    Sym sym_primitive = symtab().get("Sym");

    bool done = false;

    VmCode::code_t* code = &(vm.code.codes[label]);

    while (!done) {

        if (ip >= code->size()) {
            throw std::runtime_error("Sanity error: instruction pointer out of bounds.");
        }

        Opcode& c = (*code)[ip];

        if (verbose) {
            std::cout << ">" << ip << " " << opcodename(c.op) << "(" << c.arg.inte << ") "
                      << " ||\t\t\t";
            for (const auto& ii : vm.stack) {
                std::cout << " " << ii.inte << ":" << symtab().get(ii.uint);
            }
            std::cout << std::endl;
        }

        switch (c.op) {
        case NOOP:
            break;
            
        case PUSH:
            vm.stack.push_back(c.arg);
            break;

        case POP:
            vm.stack.pop_back();
            break;

        case SWAP: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v2);
            vm.stack.push_back(v1);
            break;
        }

        case DUP: {
            Val v = vm.pop();
            vm.stack.push_back(v);
            vm.stack.push_back(v);
            break;
        }

        case PUSH_DUP: {
            Val v = vm.stack.back();
            vm.stack.push_back(c.arg);
            vm.stack.push_back(v);
            break;
        }

        case EXIT: {
            if (vm.frame.size() == 0) {
                return;
            } else {
                auto symip = vm.frame.back();
                vm.frame.pop_back();
                label = symip.first;
                code = &(vm.code.codes[label]);
                ip = symip.second;
                continue;
            }
        }

        case CALL: {
            Val totype = vm.pop();
            Val fromtype = vm.pop();

            const Shape& shape = vm.shapes.get(fromtype.uint);

            Struct tmp;
            auto tope = vm.stack.end();
            auto topb = tope - shape.size();
            tmp.v.assign(topb, tope);
            vm.stack.resize(vm.stack.size() - shape.size());
            
            auto i = vm.code.codes.find(label);

            if (i != vm.code.codes.end()) {

                vm.frame.push_back(std::make_pair(label, ip+1));
                label = std::make_pair(fromtype.uint, totype.uint);
                code = &(i->second);
                ip = 0;
                continue;

            } else {
                Struct ret;

                auto j = vm.func_callbacks.find(std::make_pair(fromtype.uint, totype.uint));

                if (j == vm.func_callbacks.end()) {
                    throw std::runtime_error("Callback for '" + 
                                             symtab().get(fromtype.uint) + "->" +
                                             symtab().get(totype.uint) + "' undefined");
                }

                (j->second)(vm.shapes, shape, vm.shapes.get(totype.uint), tmp, ret);

                vm.stack.insert(vm.stack.end(), ret.v.begin(), ret.v.end());
            }

            break;
        }
            
        case ADD_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.inte + v2.inte);
            break;
        }

        case SUB_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.inte - v2.inte);
            break;
        }

        case MUL_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.inte * v2.inte);
            break;
        }

        case DIV_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.inte / v2.inte);
            break;
        }

        case NEG_INT: {
            Val v = vm.pop();
            vm.stack.push_back(-v.inte);
            break;
        }

        case ADD_UINT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.uint + v2.uint);
            break;
        }

        case SUB_UINT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.uint - v2.uint);
            break;
        }

        case MUL_UINT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.uint * v2.uint);
            break;
        }

        case DIV_UINT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.uint / v2.uint);
            break;
        }

        case ADD_REAL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.real + v2.real);
            break;
        }

        case SUB_REAL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.real - v2.real);
            break;
        }

        case MUL_REAL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.real * v2.real);
            break;
        }

        case DIV_REAL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.real / v2.real);
            break;
        }

        case NEG_REAL: {
            Val v = vm.pop();
            vm.stack.push_back(-v.real);
            break;
        }

        case BAND: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.uint & v2.uint);
            break;
        }

        case BOR: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.uint | v2.uint);
            break;
        }

        case BNOT: {
            Val v1 = vm.pop();
            vm.stack.push_back(~v1.uint);
            break;
        }

        case BXOR: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.uint ^ v2.uint);
            break;
        }

        case BSHL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.uint << v2.uint);
            break;
        }

        case BSHR: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(v1.uint >> v2.uint);
            break;
        }

        case EQ_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.inte == v2.inte));
            break;
        }

        case LT_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.inte < v2.inte));
            break;
        }

        case LTE_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.inte <= v2.inte));
            break;
        }

        case GT_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.inte > v2.inte));
            break;
        }

        case GTE_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.inte >= v2.inte));
            break;
        }

        case EQ_UINT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.uint == v2.uint));
            break;
        }

        case LT_UINT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.uint < v2.uint));
            break;
        }

        case LTE_UINT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.uint <= v2.uint));
            break;
        }

        case GT_UINT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.uint > v2.uint));
            break;
        }

        case GTE_UINT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.uint >= v2.uint));
            break;
        }

        case EQ_REAL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.real == v2.real));
            break;
        }

        case LT_REAL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.real < v2.real));
            break;
        }

        case LTE_REAL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.real <= v2.real));
            break;
        }

        case GT_REAL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.real > v2.real));
            break;
        }

        case GTE_REAL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.real >= v2.real));
            break;
        }

        case NEW_SHAPE: {
            vm.tmp_shape = Shape();
            break;
        }

        case DEF_FIELD: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.tmp_shape.add_field(v1.uint, (Type)v2.uint);
            break;
        }

        case DEF_STRUCT_FIELD: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            const Shape& sh = vm.shapes.get(v2.uint);
            vm.tmp_shape.add_field(v1.uint, STRUCT, v2.uint, sh.size());
            break;
        }

        case DEF_SHAPE: {
            Val v = vm.pop();
            vm.shapes.add(v.uint, vm.tmp_shape);
            break;
        }

        case NEW_STRUCT: {
            Val v = vm.pop();
            vm.stack.insert(vm.stack.end(), v.uint, Val());
            break;
        }

        case SET_FIELDS: {
            Val strusize = vm.pop();
            Val offs_end = vm.pop();
            Val offs_beg = vm.pop();

            size_t topsize = (offs_end.uint - offs_beg.uint);
            auto tope = vm.stack.end();
            auto topi = tope - topsize;
            auto stri = topi - strusize.uint + offs_beg.uint;

            for (auto i = topi; i != tope; ++i, ++stri) {
                *stri = *i;
            }

            vm.stack.resize(vm.stack.size() - topsize);
            break;
        } 

        case SYSCALL_STRUCT: {
            Val shapeid = vm.pop();
            const Shape& shape = vm.shapes.get(shapeid.uint);

            Struct tmp;
            auto tope = vm.stack.end();
            auto topb = tope - shape.size();
            tmp.v.assign(topb, tope);
            
            auto i = vm.callbacks.find(shapeid.uint);
            if (i == vm.callbacks.end()) {
                throw std::runtime_error("Callback for '" + 
                                         symtab().get(shapeid.uint) + 
                                         "' undefined");
            }

            (i->second)(vm.shapes, shape, tmp);

            vm.stack.resize(vm.stack.size() - shape.size());
            break;
        }

        case SYSCALL_PRIMITIVE: {
            Sym totype = vm.pop().uint;
            Sym fromtype = vm.pop().uint;
            Val from = vm.pop();
            Val ret;

            if (fromtype == int_primitive && totype == real_primitive) {
                ret.real = (Real)from.inte;

            } else if (fromtype == real_primitive && totype == int_primitive) {
                ret.inte = (Int)from.real;

            } else if (fromtype == int_primitive && totype == sym_primitive) {
                ret.uint = symtab().get(std::string(1, (char)from.inte));

            } else if (fromtype == uint_primitive && totype == sym_primitive) {
                ret.uint = symtab().get(std::string(1, (unsigned char)from.uint));

            } else if ((fromtype == bool_primitive && totype == int_primitive) ||
                       (fromtype == bool_primitive && totype == uint_primitive) || 
                       (fromtype == int_primitive && totype == bool_primitive) ||
                       (fromtype == uint_primitive && totype == bool_primitive)) {
                ret.uint = (bool)from.uint;

            } else {
                throw std::runtime_error("Don't know how to convert " + 
                                         symtab().get(fromtype) + "->" +
                                         symtab().get(totype));
            }
            
            vm.stack.push_back(ret);
            break;
        }

        }

        ++ip;
    }
}

}


#endif


