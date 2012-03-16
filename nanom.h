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

    bool has_shape(Sym shapeid) const {
        return (shapes.count(shapeid) != 0);
    }

    void add(Sym shapeid, const Shape& sh) {

        auto i = shapes.find(shapeid);

        if (i != shapes.end())
            throw std::runtime_error("Cannot redefine shape: " + symtab().get(shapeid));

        shapes.insert(i, std::make_pair(shapeid, sh));
    }

    const Shape& get(const std::string& s) const {
        return get(symtab().get(s));
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

    IF_FAIL,
    IF_NOT_FAIL,
    POP_FRAME,
    POP_FRAMEHEAD,
    FAIL,
    IF,
    IF_NOT,

    CALL,
    CALL_LIGHT,
    EXIT,

    NEW_SHAPE,
    DEF_FIELD,
    DEF_STRUCT_FIELD,
    DEF_SHAPE,

    NEW_STRUCT,
    SET_FIELDS,
    GET_FRAMEHEAD_FIELDS,

    /***/

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

    INT_TO_REAL,
    REAL_TO_INT,
    INT_TO_CHAR,
    UINT_TO_CHAR

};


struct Opcode {
    op_t op;
    Val arg;

    Opcode(op_t o = NOOP, Val a = (UInt)0) : op(o), arg(a) {}
};


struct label_t {
    Sym name;
    Sym fromshape;
    Sym toshape;
    label_t(Sym a=0, Sym b=0, Sym c=0) : name(a), fromshape(b), toshape(c) {}
    
    bool operator==(const label_t& l) const {
        return (name == l.name && fromshape == l.fromshape && toshape == l.toshape);
    }
};

}


namespace std {
template <>
struct hash<nanom::label_t> {
    size_t operator()(const nanom::label_t& l) const {
        return (hash<nanom::Sym>()(l.name) ^ 
                hash<nanom::Sym>()(l.fromshape) ^
                hash<nanom::Sym>()(l.toshape));
    }
};
}


namespace nanom {

struct VmCode {
    typedef std::vector<Opcode> code_t;
     
    std::unordered_map<label_t, code_t> codes;

    static label_t toplevel_label() {
        Sym none = symtab().get("");
        return label_t(none, none, none);
    }
};


typedef bool (*callback_t)(const Shapes&, const Shape&, const Shape&, const Struct&, Struct&);


struct Vm {

    struct frame_t {
        label_t prev_label;
        size_t prev_ip;
        size_t stack_ix;
        size_t struct_size;

        frame_t() : prev_ip(0), stack_ix(0), struct_size(0) {}
        frame_t(const label_t& l, size_t i, size_t s, size_t ss) : 
            prev_label(l), prev_ip(i), stack_ix(s), struct_size(ss) {}
    };

    std::vector<Val> stack;
    std::vector<frame_t> frame;
    bool failbit;

    VmCode& code;

    Shapes shapes;

    std::unordered_map<label_t, callback_t> callbacks;

    Shape tmp_shape;


    Vm(VmCode& c) : failbit(false), code(c) {}

    void register_callback(label_t s, callback_t cb) {
        callbacks[s] = cb;
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
        m[(size_t)IF] = "IF";
        m[(size_t)IF_NOT] = "IF_NOT";
        m[(size_t)IF_FAIL] = "IF_FAIL";
        m[(size_t)IF_NOT_FAIL] = "IF_NOT_FAIL";
        m[(size_t)POP_FRAME] = "POP_FRAME";
        m[(size_t)POP_FRAMEHEAD] = "POP_FRAMEHEAD";
        m[(size_t)FAIL] = "FAIL";
        m[(size_t)CALL] = "CALL";
        m[(size_t)CALL_LIGHT] = "CALL_LIGHT";
        m[(size_t)EXIT] = "EXIT";
        m[(size_t)NEW_SHAPE] = "NEW_SHAPE";
        m[(size_t)DEF_FIELD] = "DEF_FIELD";
        m[(size_t)DEF_STRUCT_FIELD] = "DEF_STRUCT_FIELD";
        m[(size_t)DEF_SHAPE] = "DEF_SHAPE";
        m[(size_t)NEW_STRUCT] = "NEW_STRUCT";
        m[(size_t)SET_FIELDS] = "SET_FIELDS";
        m[(size_t)GET_FRAMEHEAD_FIELDS] = "GET_FRAMEHEAD_FIELDS";
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
        m[(size_t)INT_TO_REAL] = "INT_TO_REAL";
        m[(size_t)REAL_TO_INT] = "REAL_TO_INT";
        m[(size_t)INT_TO_CHAR] = "INT_TO_CHAR";
        m[(size_t)UINT_TO_CHAR] = "UINT_TO_CHAR";
        
        n["NOOP"] = NOOP;
        n["PUSH"] = PUSH;
        n["POP"] = POP;
        n["SWAP"] = SWAP;
        n["IF"] = IF;
        n["IF_NOT"] = IF_NOT;
        n["IF_FAIL"] = IF_FAIL;
        n["IF_NOT_FAIL"] = IF_NOT_FAIL;
        n["POP_FRAME"] = POP_FRAME;
        n["POP_FRAMEHEAD"] = POP_FRAMEHEAD;
        n["FAIL"] = FAIL;
        n["CALL"] = CALL;
        n["CALL_LIGHT"] = CALL_LIGHT;
        n["EXIT"] = EXIT;
        n["NEW_SHAPE"] = NEW_SHAPE;
        n["DEF_FIELD"] = DEF_FIELD;
        n["DEF_STRUCT_FIELD"] = DEF_STRUCT_FIELD;
        n["DEF_SHAPE"] = DEF_SHAPE;
        n["NEW_STRUCT"] = NEW_STRUCT;
        n["SET_FIELDS"] = SET_FIELDS;
        n["GET_FRAMEHEAD_FIELDS"] = GET_FRAMEHEAD_FIELDS;
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
        n["INT_TO_REAL"] = INT_TO_REAL;
        n["REAL_TO_INT"] = REAL_TO_INT;
        n["INT_TO_CHAR"] = INT_TO_CHAR;
        n["UINT_TO_CHAR"] = UINT_TO_CHAR;
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
                   label_t label = VmCode::toplevel_label(), 
                   size_t ip = 0, 
                   bool verbose = false) {

    bool done = false;

    VmCode::code_t* code = &(vm.code.codes[label]);

    vm.frame.emplace_back(label, 0, 0, vm.stack.size());

    while (!done) {

        if (ip >= code->size()) {
            throw std::runtime_error("Sanity error: instruction pointer out of bounds.");
        }

        Opcode& c = (*code)[ip];

        if (verbose) {
            std::cout << ">" << ip << " " << opcodename(c.op) << "(" << c.arg.inte << ") "
                      << vm.failbit << " ||\t\t\t";
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

        case IF: {
            Val v = vm.pop();
            if (v.uint) {
                ip += c.arg.inte;
                continue;
            }
            break;
        }

        case IF_NOT: {
            Val v = vm.pop();
            if (!v.uint) {
                ip += c.arg.inte;
                continue;
            }
            break;
        }

        case IF_FAIL: 
            if (vm.failbit) {
                ip += c.arg.inte;
                continue;
            }
            break;

        case IF_NOT_FAIL:
            if (!vm.failbit) {
                ip += c.arg.inte;
                continue;
            }
            break;

        case POP_FRAME: {
            const auto& fp = vm.frame.back();
            auto sb = vm.stack.begin() + fp.stack_ix;
            vm.stack.erase(sb, vm.stack.end());
            break;
        }

        case POP_FRAMEHEAD: {
            const auto& fp = vm.frame.back();
            auto sb = vm.stack.begin() + fp.stack_ix;
            auto se = sb + fp.struct_size;
            vm.stack.erase(sb, se);
            break;
        }
            
        case FAIL:
            vm.failbit = true;
            // No break, do an EXIT

        case EXIT: {

            if (vm.frame.size() == 1) {
                vm.frame.pop_back();
                return;

            } else {
                const auto& fp = vm.frame.back();
                label = fp.prev_label;
                ip = fp.prev_ip;
                code = &(vm.code.codes[label]);
                vm.frame.pop_back();
                continue;
            }
        }

        case CALL: {
            Val totype = vm.pop();
            Val fromtype = vm.pop();
            Val name = vm.pop();

            const Shape& shape = vm.shapes.get(fromtype.uint);
            
            label_t l(name.uint, fromtype.uint, totype.uint);
            auto i = vm.code.codes.find(l);

            if (i != vm.code.codes.end()) {

                vm.frame.emplace_back(label, ip+1, vm.stack.size() - shape.size(), shape.size());

                vm.failbit = false;
                label = l;
                code = &(i->second);
                ip = 0;
                continue;

            } else {

                Struct tmp;
                auto tope = vm.stack.end();
                auto topb = tope - shape.size();
                tmp.v.assign(topb, tope);
                vm.stack.resize(vm.stack.size() - shape.size());

                Struct ret;

                auto j = vm.callbacks.find(l);

                if (j == vm.callbacks.end()) {
                    throw std::runtime_error("Callback '" + symtab().get(name.uint) + " " +
                                             symtab().get(fromtype.uint) + "->" +
                                             symtab().get(totype.uint) + "' undefined");
                }

                vm.failbit = !(j->second)(vm.shapes, shape, vm.shapes.get(totype.uint), tmp, ret);

                vm.stack.insert(vm.stack.end(), ret.v.begin(), ret.v.end());
            }

            break;
        }


        case CALL_LIGHT: {
            Val name = vm.pop();

            const auto& fr = vm.frame.back();

            vm.frame.emplace_back(label, ip+1, fr.stack_ix, fr.struct_size);

            vm.failbit = false;
            label.name = name.uint;
            code = &(vm.code.codes[label]);
            ip = 0;
            continue;
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

        case GET_FRAMEHEAD_FIELDS: {
            Val offs_end = vm.pop();
            Val offs_beg = vm.pop();

            const auto& fp = vm.frame.back();
            auto sb = vm.stack.begin() + fp.stack_ix;
            auto se = sb + offs_end.uint;
            sb += offs_beg.uint;

            vm.stack.insert(vm.stack.end(), sb, se);
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

        case INT_TO_REAL: {
            Val v = vm.pop();
            Val ret = (Real)v.inte;
            vm.stack.push_back(ret);
            break;
        }

        case REAL_TO_INT: {
            Val v = vm.pop();
            Val ret = (Int)v.real;
            vm.stack.push_back(ret);
            break;
        }

        case INT_TO_CHAR: {
            Val v = vm.pop();
            Val ret;
            ret.uint = symtab().get(std::string(1, (char)v.inte));
            vm.stack.push_back(ret);
            break;
        }

        case UINT_TO_CHAR: {
            Val v = vm.pop();
            Val ret;
            ret.uint = symtab().get(std::string(1, (unsigned char)v.uint));
            vm.stack.push_back(ret);
            break;
        }

        }

        ++ip;
    }
}

}


#endif


