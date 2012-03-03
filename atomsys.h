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
    };

    std::unordered_map< Sym, std::pair<size_t,typeinfo> > sym2field;
    std::vector< std::pair<Sym,typeinfo> > idx2field;

    typeinfo get_type(Sym s) const {
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

    void add_field(Sym s, Type t, Sym shape=0) {
        if (sym2field.count(s) != 0) {
            throw std::runtime_error("Cannot add duplicate field to shape.");
        }

        typeinfo ti;
        ti.type = t;
        ti.shape = shape;

        idx2field.push_back(std::make_pair(s, ti));
        sym2field[s] = std::make_pair(idx2field.size()-1, ti);
    }

    typeinfo get_type(const std::string& s) const { return get_type(symtab().get(s)); }
    Int get_index(const std::string& s) const { return get_index(symtab().get(s)); }
    void add_field(const std::string& s, Type t, Sym shape=0) { add_field(symtab().get(s), t, shape); }
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
    std::unordered_map<Sym, 
                       std::unordered_map<size_t, Struct> > structs;

    Struct& get(Sym shapeid, size_t structid) {
        auto& m = structs[shapeid];
        auto ii = m.find(structid);

        if (ii == m.end())
            throw std::runtime_error("Invalid structid");
        return ii->second;
    }

    size_t add(Sym shapeid) {

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
    typedef std::unordered_map<Sym, index_t> shape_index_t;
    shape_index_t idx;

    void add(Sym shapeid, size_t structid) {

        index_t& m = idx[shapeid];
        const Shape& sh = shapes().get(shapeid);
        const Struct& st = structs().get(shapeid, structid);

        size_t n = 0;
        for (const auto& i : sh.idx2field) {
            m.insert(m.end(), std::make_pair(std::make_pair(i.first, st.v[n]), structid));
            ++n;
        }
    }

    void del(Sym shapeid, size_t structid) {

        index_t& m = idx[shapeid];
        const Shape& sh = shapes().get(shapeid);
        const Struct& st = structs().get(shapeid, structid);

        size_t n = 0;
        for (const auto& i : sh.idx2field) {
            m.erase(std::make_pair(std::make_pair(i.first, st.v[n]), structid));
            ++n;
        }
    }

    void change_field(Sym shapeid, size_t structid, Sym field, Val newval) {

        index_t& m = idx[shapeid];
        const Shape& sh = shapes().get(shapeid);
        Struct& st = structs().get(shapeid, structid);
        Int fi = sh.get_index(field);

        if (fi < 0)
            throw std::runtime_error("Invalid field in change_field");

        m.erase(std::make_pair(std::make_pair(field, st.v[fi]), structid));
        st.v[fi] = newval;
        m.insert(m.end(), std::make_pair(std::make_pair(field, newval), structid));
    }

    std::pair<index_t::const_iterator, index_t::const_iterator>
    find(Sym shapeid, Sym field, Val val) {
        return idx[shapeid].equal_range(std::make_pair(field, val));
    }
};


inline Index& index() {
    static Index _ret;
    return _ret;
}




enum op_t {
    NOOP = 0,

    PUSH,
    POP,
    SWAP,
    DUP,
    PUSH_DUP,

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
    GTE_REAL

    NEW_SHAPE,
    ADD_FIELD,
    ADD_STRUCT_FIELD,
    DEF_SHAPE

    NEW_STRUCT,

    SET_FIELD,
    GET_FIELD,

    ADD_INDEX,
    DEL_INDEX,
    INDEX_CHANGE_FIELD,
    INDEX_FIND,
    INDEX_FIND2
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
    std::unordered_map<op_t,std::string> m;
    std::unordered_map<std::string,op_t> n;
    _mapper() {
        m[NOOP] = "NOOP";
        m[PUSH] = "PUSH";
        m[POP] = "POP";
        m[SWAP] = "SWAP";
        m[DUP] = "DUP";
        m[PUSH_DUP] = "PUSH_DUP";
        m[EXIT] = "EXIT";
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
        m[ADD_STRUCT_FIELD] = "ADD_STRUCT_FIELD";
        m[DEF_SHAPE] = "DEF_SHAPE";
        m[NEW_STRUCT] = "NEW_STRUCT";
        m[SET_FIELD] = "SET_FIELD";
        m[GET_FIELD] = "GET_FIELD";
        m[ADD_INDEX] = "ADD_INDEX";
        m[DEL_INDEX] = "DEL_INDEX";
        m[INDEX_CHANGE_FIELD] = "INDEX_CHANGE_FIELD";
        m[INDEX_FIND] = "INDEX_FIND";
        m[INDEX_FIND2] = "INDEX_FIND2";
        
        n["NOOP"] = NOOP;
        n["PUSH"] = PUSH;
        n["POP"] = POP;
        n["SWAP"] = SWAP;
        n["DUP"] = DUP;
        n["PUSH_DUP"] = PUSH_DUP;
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
        n["ADD_FIELD"] = ADD_FIELD;
        n["ADD_STRUCT_FIELD"] = ADD_STRUCT_FIELD;
        n["DEF_SHAPE"] = DEF_SHAPE;
        n["NEW_STRUCT"] = NEW_STRUCT;
        n["SET_FIELD"] = SET_FIELD;
        n["GET_FIELD"] = GET_FIELD;
        n["ADD_INDEX"] = ADD_INDEX;
        n["DEL_INDEX"] = DEL_INDEX;
        n["INDEX_CHANGE_FIELD"] = INDEX_CHANGE_FIELD;
        n["INDEX_FIND"] = INDEX_FIND;
        n["INDEX_FIND2"] = INDEX_FIND2;
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


inline void vm_run(Vm& vm, Sym label, size_t ip = 0) {

    bool done = false;

    Vm::code_t code* = &(vm.code.codes[label]);

    while (!done) {

        if (ip >= code->size()) {
            throw std::runtime_error("Sanity error: instruction pointer out of bounds.");
        }

        Opcode& c = (*code)[ip];

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

        case ADD_STRUCT_FIELD: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.tmp_shape.add_field(v1.uint, STRUCT, v2.uint);
            break;
        }

        case DEF_SHAPE: {
            Val v = vm.pop();
            shapes().add(v.uint, vm.tmp_shape);
            break;
        }

        case NEW_STRUCT: {
            Val v = vm.pop();
            size_t sh = structs().add(v.uint);
            vm.stack.push_back(sh);
            break;
        }

        case GET_FIELD: {
            Val v3 = vm.pop();
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            const Struct& st = structs().get(v1.uint, v2.uint);
            vm.stack.push_back(st.get_field(v3.uint));
            break;
        }
            
        case SET_FIELD: {
            Val v4 = vm.pop();
            Val v3 = vm.pop();
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            const Struct& st = structs().get(v1.uint, v2.uint);
            st.set_field(v3.uint, v4);
            break;
        }
            
        case INDEX_ADD: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            Index& ix = index();
            ix.add(v1.uint, v2.uint);
            break;
        }

        case INDEX_DEL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            Index& ix = index();
            ix.del(v1.uint, v2.uint);
            break;
        }
            
        case INDEX_CHANGE_FIELD: {
            Val v4 = vm.pop();
            Val v3 = vm.pop();
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            Index& ix = index();
            ix.change_field(v1.uint, v2.uint, v3.uint, v5);
            break;
        }

        case INDEX_FIND: {
            Val v3 = vm.pop();
            Val v2 = vm.pop();
            Val v1 = vm.pop();

            auto range = index().find(v1.uint, v2.uint, v3);

            while (range.first != range.second) {

                Vm vm2(vm.code);
                vm2.stack.push_back(range.first->second);
                vm_run(vm2, c.arg.uint);

                ++(range.first);
            }

            return;
        }

        case INDEX_FIND2: {
            Val v5 = vm.pop();
            Val v4 = vm.pop();
            Val v3 = vm.pop();
            Val v2 = vm.pop();
            Val v1 = vm.pop();

            auto range1 = index().find(v1.uint, v2.uint, v3);
            auto range2 = index().find(v1.uint, v4.uint, v5);

            while (range1.first != range1.second && range2.first != range2.second) {

                bool lt1 = (*(range1.first) < *(range2.second));
                bool lt2 = (*(range2.first) < *(range1.second));

                if (lt1) {
                    ++(range1.first);

                } else if (lt2) {
                    ++(range2.first);

                } else {
                    Vm vm2(vm.code);
                    vm2.stack.push_back(range1.first->second);
                    vm_run(vm2, c.arg.uint);
                }
            }

            return;
        }


        }

        ++ip;
    }
}



}


#endif


/*

NEW_SHAPE
PUSH('slot')
PUSH(SYM)
ADD_FIELD
PUSH('name')
PUSH(SYM
ADD_FIELD
PUSH('coord_x')
PUSH(INT)
ADD_FIELD
PUSH('coord_y')
PUSH(INT)
ADD_FIELD
PUSH('invslot')
PUSH(SYM)
ADD_FIELD
PUSH('item')
DEF_SHAPE

PUSH('item')
NEW_STRUCT

PUSH_DUP('item')
PUSH('slot')
PUSH('b')
SET_FIELD

PUSH_DUP('item')
PUSH('coord_x')
PUSH(-1)
SET_FIELD

PUSH_DUP('item')
PUSH('coord_y')
PUSH(-1)
SET_FIELD

PUSH_DUP('item')
PUSH('invslot')
PUSH(-1)
SET_FIELD

PUSH_DUP('item')
INDEX_ADD

POP

...




 */
