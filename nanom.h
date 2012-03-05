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

        typeinfo() : type(NONE), shape(0), ix_from(0), ix_to(0) {}
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
            return std::make_pair(0,0);
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

    const Shape& get(Sym shapeid) {
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
    std::vector<Val> v;

    Struct(size_t n = 0) {
        v.resize(n);
    }

    const Val& get_field(size_t i) const {
        return v.at(i);
    }

    void set_field(size_t i, Val val) {
        v[i] = val;
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

    INT_TO_REAL,
    REAL_TO_INT,

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

    SYSCALL_STRUCT

};


struct Opcode {
    op_t op;
    Val arg;
};

struct VmCode {
    typedef std::vector<Opcode> code_t;
    std::unordered_map<Sym, code_t> codes;
};


struct Vm {

    std::vector<Val> stack;
    std::vector< std::pair<Sym,size_t> > frame;

    VmCode& code;

    Shapes shapes;

    Shape tmp_shape;

    Vm(VmCode& c) : code(c) {}

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
        m[(size_t)INT_TO_REAL] = "INT_TO_REAL";
        m[(size_t)REAL_TO_INT] = "REAL_TO_INT";
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
        n["INT_TO_REAL"] = INT_TO_REAL;
        n["REAL_TO_INT"] = REAL_TO_INT;
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


inline void vm_run(Vm& vm, Sym label, size_t ip = 0, bool verbose = false) {

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
                std::cout << " " << ii.inte;
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
            vm.frame.push_back(std::make_pair(label, ip+1));
            label = c.arg.uint;
            code = &(vm.code.codes[label]);
            ip = 0;
            continue;
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

        case INT_TO_REAL:
            vm.stack.back().real = (Real)vm.stack.back().inte;
            break;

        case REAL_TO_INT:
            vm.stack.back().inte = (Int)vm.stack.back().real;
            break;

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

            for (auto i = topi; i != tope; ++i) {
                *stri = *i;
            }

            vm.stack.resize(vm.stack.size() - topsize);
            break;
        } 

        case SYSCALL_STRUCT: {
            Val strusize = vm.pop();
            Struct tmp;
            auto tope = vm.stack.end();
            auto topb = tope - strusize.uint;
            tmp.v.assign(topb, tope);
            //
            vm.stack.resize(vm.stack.size() - strusize.uint);
            break;
        }

        }

        ++ip;
    }
}



}


#endif


