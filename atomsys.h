#ifndef __ATOMSYS_H
#define __ATOMSYS_H

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



namespace atomsys {

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
    NONE = 0,
    SYMBOL,
    INT, 
    UINT,
    REAL,
    STRUCT
};

struct Shape {
    std::unordered_map< Sym, std::pair<size_t,Type> > sym2field;
    std::vector< std::pair<Sym,Type> > idx2field;

    Type get_type(Sym s) const {
        auto i = sym2field.find(s);
        if (i == sym2field.end())
            return NONE;
        return i->second.second;
    }

    Int get_index(Sym s) const {
        auto i = sym2field.find(s);
        if (i == sym2field.end())
            return -1;
        return i->second.first;
    }

    void add_field(Sym s, Type t) {
        if (sym2field.count(s) != 0) {
            throw std::runtime_error("Cannot add duplicate field to shape.");
        }

        idx2field.push_back(std::make_pair(s, t));
        sym2field[s] = std::make_pair(idx2field.size()-1, t);
    }

    Type get_type(const std::string& s) const { return get_type(symtab().get(s)); }
    Int get_index(const std::string& s) const { return get_index(symtab().get(s)); }
    void add_field(const std::string& s, Type t) { add_field(symtab().get(s), t); }
};

struct Shapes {
    std::vector<Shape> shapes;

    const Shape& get(size_t shapeid) {
        return shapes.at(shapeid);
    }

    size_t add(const Shape& sh) {
        shapes.push_back(sh);
        return shapes.size()-1;
    }
};

inline Shapes& shapes() {
    static Shapes _ret;
    return _ret;
}


struct Struct {
    std::vector<Val> v;

    const Val& get_field(size_t i) const {
        return v.at(i);
    }

    void set_field(size_t i, Val val) {
        v[i] = val;
    }
};

struct Structs {
    std::unordered_map<size_t, 
                       std::unordered_map<size_t, Struct> > structs;

    Struct& get(size_t shapeid, size_t structid) {
        auto& m = structs[shapeid];
        auto ii = m.find(structid);

        if (ii == m.end())
            throw std::runtime_error("Invalid structid");
        return ii->second;
    }

    size_t add(size_t shapeid) {

        const Shape& sh = shapes().get(shapeid);
        size_t n = sh.idx2field.size();

        auto& m = structs[shapeid];
        size_t ret = m.size()+1;
        Struct& s = m[ret];
        s.v.resize(n);

        return ret;
    }
};

inline Shapes& structs() {
    static Structs _ret;
    return _ret;
}


struct Index {
    typedef std::unordered_multimap< std::pair<Sym,Val>, size_t > index_t;
    index_t idx;

    void add(size_t shapeid, size_t structid) {

        const Struct& st = structs().get(shapeid, structid);

        size_t n = 0;
        for (const auto& i : st.shape.idx2field) {
            idx.insert(idx.end(), std::make_pair(std::make_pair(i.first, st.v[n]), structid));
            ++n;
        }
    }

    void del(size_t shapeid, size_t structid) {

        const Struct& st = structs().get(shapeid, structid);

        size_t n = 0;
        for (const auto& i : st.shape.idx2field) {
            idx.erase(std::make_pair(std::make_pair(i.first, st.v[n]), structid));
            ++n;
        }
    }

    void change_field(size_t shapeid, size_t structid, Sym field, Val newval) {

        Shape& sh = shapes().get(shapeid);
        Struct& st = structs().get(shapeid, structid);
        Int fi = sh.get_index(field);

        if (fi < 0)
            throw std::runtime_error("Invalid field in change_field");

        idx.erase(std::make_pair(std::make_pair(field, st.v[fi]), structid));
        st.v[fi] = newval;
        idx.insert(idx.end(), std::make_pair(std::make_pair(field, newval), structid));
    }
};


struct Indexes {
    std::unordered_map<size_t, Index> inxs;

    Index& get(size_t n) {
        return inxs[n];
    }
};

inline Shapes& indexes() {
    static Indexes _ret;
    return _ret;
}




enum op_t {
    NOOP = 0,

    PUSH,
    POP,
    SWAP,
    DUP,

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
    GTE_REAL

    NEW_SHAPE,
    ADD_FIELD,
    ADD_SHAPE

    NEW_STRUCT,
    ADD_STRUCT,

    GET_STRUCT,
    SET_FIELD,
    GET_FIELD,

    INDEX_ADD,
    INDEX_DEL,
    INDEX_CHANGE_FIELD
};


struct Opcode {
    op_t op;
    Val arg;
};

struct Vm {

    std::vector<Val> stack;
    std::vector<Opcode> code;

    Shape tmp_shape;

    Vm() {}

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
    std::unordered_map<op_t,std::string> m;
    std::unordered_map<std::string,op_t> n;
    _mapper() {
        m[NOOP] = "NOOP";
        m[PUSH] = "PUSH";
        m[POP] = "POP";
        m[SWAP] = "SWAP";
        m[DUP] = "DUP";
        m[ADD_INT] = "ADD_INT";
        m[SUB_INT] = "SUB_INT";
        m[MUL_INT] = "MUL_INT";
        m[DIV_INT] = "DIV_INT";
        m[NEG_INT] = "NEG_INT";
        m[ADD_UINT] = "ADD_UINT";
        m[SUB_UINT] = "SUB_UINT";
        m[MUL_UINT] = "MUL_UINT";
        m[DIV_UINT] = "DIV_UINT";
        m[ADD_REAL] = "ADD_REAL";
        m[SUB_REAL] = "SUB_REAL";
        m[MUL_REAL] = "MUL_REAL";
        m[DIV_REAL] = "DIV_REAL";
        m[NEG_REAL] = "NEG_REAL";
        m[BAND] = "BAND";
        m[BOR] = "BOR";
        m[BNOT] = "BNOT";
        m[BXOR] = "BXOR";
        m[BSHL] = "BSHL";
        m[BSHR] = "BSHR";
        m[INT_TO_REAL] = "INT_TO_REAL";
        m[REAL_TO_INT] = "REAL_TO_INT";
        m[EQ_INT] = "EQ_INT";
        m[LT_INT] = "LT_INT";
        m[LTE_INT] = "LTE_INT";
        m[GT_INT] = "GT_INT";
        m[GTE_INT] = "GTE_INT";
        m[EQ_UINT] = "EQ_UINT";
        m[LT_UINT] = "LT_UINT";
        m[LTE_UINT] = "LTE_UINT";
        m[GT_UINT] = "GT_UINT";
        m[GTE_UINT] = "GTE_UINT";
        m[EQ_REAL] = "EQ_REAL";
        m[LT_REAL] = "LT_REAL";
        m[LTE_REAL] = "LTE_REAL";
        m[GT_REAL] = "GT_REAL";
        m[GTE_REAL] = "GTE_REAL";
        m[NEW_SHAPE] = "NEW_SHAPE";
        m[ADD_FIELD] = "ADD_FIELD";
        m[ADD_SHAPE] = "ADD_SHAPE";
        m[NEW_STRUCT] = "NEW_STRUCT";
        m[ADD_STRUCT] = "ADD_STRUCT";
        m[GET_STRUCT] = "GET_STRUCT";
        m[SET_FIELD] = "SET_FIELD";
        m[GET_FIELD] = "GET_FIELD";
        m[INDEX_ADD] = "INDEX_ADD";
        m[INDEX_DEL] = "INDEX_DEL";
        m[INDEX_CHANGE_FIELD] = "INDEX_CHANGE_FIELD";

        n["NOOP"] = NOOP;
        n["PUSH"] = PUSH;
        n["POP"] = POP;
        n["SWAP"] = SWAP;
        n["DUP"] = DUP;
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
        n["ADD_FIELD"] = ADD_FIELD;
        n["ADD_SHAPE"] = ADD_SHAPE;
        n["NEW_STRUCT"] = NEW_STRUCT;
        n["ADD_STRUCT"] = ADD_STRUCT;
        n["GET_STRUCT"] = GET_STRUCT;
        n["SET_FIELD"] = SET_FIELD;
        n["GET_FIELD"] = GET_FIELD;
        n["INDEX_ADD"] = INDEX_ADD;
        n["INDEX_DEL"] = INDEX_DEL;
        n["INDEX_CHANGE_FIELD"] = INDEX_CHANGE_FIELD;

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


inline void vm_run(Vm& vm, size_t ip) {

    bool done = false;

    while (!done) {

        if (ip >= vm.code.size()) {
            throw std::runtime_error("Sanity error: instruction pointer out of bounds.");
        }

        Opcode& c = vm.code[ip];

        /*
        std::cout << "/" << ip << " " << opcodename(c.op) << "(" << c.arg.inte << ") "
                  << " ||\t\t\t";
        for (const auto& ii : vm.stack) {
            std::cout << " " << ii.inte;
        }
        std::cout << std::endl;
        */

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

        case GT_INT: {
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

        case GT_UINT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.uinte >= v2.uint));
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

        case GT_REAL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back((Int)(v1.reale >= v2.real));
            break;
        }

        case NEW_SHAPE: {
            vm.tmp_shape = Shape();
            break;
        }

        case ADD_FIELD: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.tmp_shape.add_field(v1.uint, v2.uint);
            break;
        }

        case ADD_SHAPE: {
            vm.stack.push_back(shapes().add(vm.tmp_shape));
            break;
        }

    NEW_STRUCT,
    ADD_STRUCT,

    GET_STRUCT,
    SET_FIELD,
    GET_FIELD,

    INDEX_ADD,
    INDEX_DEL,
    INDEX_CHANGE_FIELD


        }

        ++ip;
    }
}



}


#endif
