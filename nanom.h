#ifndef __NANOM_H
#define __NANOM_H

#include <ctype.h>

#include <cstdint>
#include <stdexcept>

#include <vector>
#include <unordered_map>

namespace nanom {

typedef int64_t Int;
typedef uint64_t UInt;
typedef double Real;

union Val {
    Int inte;
    UInt uint;
    Real real;

    Val() {}
    Val(Int i)  { inte = i; }
    Val(UInt i) { uint = i; }
    Val(Real i) { real = i; }
};

enum op_t {
    NOOP = 0,

    PUSH,
    TO_REG,
    COPY_TO_REG,
    FROM_REG,
    POP,

    CMP_INT,
    CMP_UINT,
    CMP_REAL,
    
    PUSH_IP,
    JUMP,
    DJUMP,
    JUMP_IF,
    DJUMP_IF,

    SYSCALL,
    EXIT,

    INT_TO_REAL,
    REAL_TO_INT,

    ADD_INT,
    SUB_INT,
    MUL_INT,
    DIV_INT
};


struct Opcode {
    op_t op;
    Val arg;
};

struct Vm;

typedef void (*syscall_callback_t)(Vm&);

struct Vm {
    static const size_t NUM_REGS = 128;

    std::vector<Val> stack;
    std::vector<Val> regs;

    std::vector<Opcode> code;

    std::unordered_map<size_t, std::string> symtab;

    std::unordered_map<size_t, syscall_callback_t> syscalls;

    Vm() {
        regs.resize(NUM_REGS);
    }

    void register_syscall(size_t n, syscall_callback_t c) {
        syscalls[n] = c;
    }

    void syscall(size_t n) {
        auto i = syscalls.find(n);

        if (i == syscalls.end())
            throw std::runtime_error("Unknown syscall index.");

        i->second(*this);
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

template <typename T>
Int _cmp(T a, T b) {
    if (a < b) return -1;
    else if (a > b) return 1;
    return 0;
}

template <typename T>
T _add(T a, T b) {
    return (a + b);
}

template <typename T>
T _sub(T a, T b) {
    return (a - b);
}

template <typename T>
T _mul(T a, T b) {
    return (a * b);
}

template <typename T>
T _div(T a, T b) {
    return (a / b);
}


}


inline void vm_run(Vm& vm, size_t ip) {

    bool done = false;

    while (!done) {

        if (ip >= vm.code.size()) {
            throw std::runtime_error("Sanity error: instruction pointer out of bounds.");
        }

        Opcode& c = vm.code[ip];

        switch (c.op) {
        case NOOP:
            break;
            
        case PUSH:
            vm.stack.push_back(c.arg);
            break;

        case TO_REG:
            vm.regs[c.arg.inte] = vm.pop();
            break;

        case COPY_TO_REG:
            vm.regs[c.arg.inte] = vm.stack.back();
            break;

        case FROM_REG:
            vm.stack.push_back(vm.regs[c.arg.inte]);
            break;

        case POP:
            vm.stack.pop_back();
            break;

        case CMP_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(_cmp(v1.inte, v2.inte));
            break;
        }

        case CMP_UINT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(_cmp(v1.uint, v2.uint));
            break;
        }

        case CMP_REAL: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(_cmp(v1.real, v2.real));
            break;
        }

        case PUSH_IP:
            vm.stack.push_back((UInt)ip + 1 + c.arg.inte);
            break;

        case JUMP:
            ip = c.arg.uint;
            continue;

        case JUMP_IF: {
            Val v = vm.pop();
            if (v.inte != 0) {
                ip = c.arg.uint;
                continue;
            }
            break;
        }
         
        case DJUMP: {
            Val offs = vm.pop();
            ip = offs.uint;
            continue;
        }

        case DJUMP_IF: {
            Val offs = vm.pop();
            Val v = vm.pop();
            if (v.inte != 0) {
                ip = offs.uint;
                continue;
            }
            break;
        }

        case SYSCALL: {
            vm.syscall(c.arg.uint);
            break;
        }

        case EXIT:
            done = true;
            break;

        case INT_TO_REAL:
            vm.stack.back().real = (Real)vm.stack.back().inte;
            break;

        case REAL_TO_INT:
            vm.stack.back().inte = (Int)vm.stack.back().real;
            break;

        case ADD_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(_add(v1.inte, v2.inte));
            break;
        }

        case SUB_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(_sub(v1.inte, v2.inte));
            break;
        }

        case MUL_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(_mul(v1.inte, v2.inte));
            break;
        }

        case DIV_INT: {
            Val v2 = vm.pop();
            Val v1 = vm.pop();
            vm.stack.push_back(_div(v1.inte, v2.inte));
            break;
        }

        }

        ++ip;
    }
}



enum opcode_arg_t {
    NONE = 0,
    VAL,
    UINT,
    ADDR
};


inline Int string_to_int(const std::string& arg, unsigned char end) {
    const char* b = arg.c_str();
    const char* e = NULL;
    Int ret = ::strtoll((char*)b, (char**)&e, 0);

    if (*e != end) 
        throw std::runtime_error("Invalid signed integer number: " + arg);
    return ret;
}

inline UInt string_to_uint(const std::string& arg, unsigned char end) {
    const char* b = arg.c_str();
    const char* e = NULL;
    UInt ret = ::strtoull((char*)b, (char**)&e, 0);

    if (*e != end) 
        throw std::runtime_error("Invalid unsigned integer number: " + arg);
    return ret;
}

inline Real string_to_real(const std::string& arg, unsigned char end) {
    const char* b = arg.c_str();
    const char* e = NULL;
    Real ret = ::strtod((char*)b, (char**)&e);

    if (*e != end) 
        throw std::runtime_error("Invalid double number: " + arg);
    return ret;
}


struct Assembler {
    std::unordered_map<std::string,Val> const_map;
    std::unordered_map<std::string,size_t> addr_map;

    struct opcodes_t {
        std::unordered_map< std::string, std::pair<op_t,opcode_arg_t> > opcodes_map;

        opcodes_t& operator()(const std::string& name, op_t op, opcode_arg_t a) {
            opcodes_map[name] = std::make_pair(op, a);
            return *this;
        }
    };

    opcodes_t opcodes_map;

    Vm& vm;

    Assembler(Vm& v) : vm(v) {

        opcodes_map
            ("PUSH", PUSH, VAL)
            ("TO_REG", TO_REG, UINT)
            ("COPY_TO_REG", COPY_TO_REG, UINT)
            ("FROM_REG", FROM_REG, UINT)
            ("POP", POP, NONE)
            ("CMP_INT", CMP_INT, NONE)
            ("CMP_UINT", CMP_UINT, NONE)
            ("CMP_REAL", CMP_REAL, NONE)
            ("JUMP", JUMP, ADDR)
            ("JUMP_IF", JUMP_IF, ADDR)
            ("DJUMP", DJUMP, NONE)
            ("DJUMP_IF", DJUMP_IF, NONE)
            ("PUSH_IP", PUSH_IP, VAL)
            ("SYSCALL", SYSCALL, UINT)
            ("EXIT", EXIT, NONE)
            ("INT_TO_REAL", INT_TO_REAL, NONE)
            ("REAL_TO_INT", REAL_TO_INT, NONE)
            ("ADD_INT", ADD_INT, NONE)
            ("SUB_INT", ADD_INT, NONE)
            ("MUL_INT", ADD_INT, NONE)
            ("DIV_INT", ADD_INT, NONE)
            ;
    }

    void vm_run(const std::string& label) {

        auto i = addr_map.find(label);

        if (i == addr_map.end())
            throw std::runtime_error("Unknown address label: " + label);

        nanom::vm_run(vm, i->second);
    }


    std::string chomp(const std::string& arg_) {
        std::string arg;
        for (auto c : arg_) {
            if (!::isspace(c)) {
                arg += c;
            }
        }
        return arg;
    }

    Val process_val(const std::string& arg_) {

        std::string arg = chomp(arg_);

        if (arg.size() == 0) 
            throw std::runtime_error("Cannot parse an empty string as value.");

        Val ret;

        unsigned char tag = arg[arg.size()-1];

        if (arg[0] == '$') {
            arg = arg.substr(1);
            auto i = const_map.find(arg);

            if (i == const_map.end()) 
                throw std::runtime_error("Unknown value constant: " + arg);

            return i->second;

        } else if (tag == 'f' || tag == 'F') {
            ret.real = string_to_real(arg, tag);
        } else if (tag == 'u' || tag == 'U') {
            ret.uint = string_to_uint(arg, tag);
        } else {
            ret.inte = string_to_int(arg, '\0');
        }

        return ret;
    }

    UInt process_uint(const std::string& arg_) {

        std::string arg = chomp(arg_);

        if (arg.size() == 0) 
            throw std::runtime_error("Cannot parse an empty string as value.");

        if (arg[0] == '$') {
            arg = arg.substr(1);
            auto i = const_map.find(arg);

            if (i == const_map.end()) 
                throw std::runtime_error("Unknown value constant: " + arg);

            return i->second.uint;
        }

        return string_to_uint(arg, '\0');
    }

    size_t process_addr(const std::string& arg_) {

        std::string arg = chomp(arg_);

        if (arg.size() == 0) 
            throw std::runtime_error("Cannot parse an empty string as value.");

        if (arg[0] == '+') {
            return vm.code.size() - 1 + process_uint(arg.substr(1));

        } else if (arg[0] == '-') {
            return vm.code.size() - 1 - process_uint(arg.substr(1));

        } else {
            auto i = addr_map.find(arg);

            if (i == addr_map.end())
                throw std::runtime_error("Unknown address label: " + arg);

            return i->second;
        }
    }

    void process_label(const std::string& arg_) {

        std::string arg = chomp(arg_);

        if (arg.size() == 0) 
            throw std::runtime_error("Cannot parse an empty string as label name.");

        addr_map[arg] = vm.code.size();
    }

    void process_const(const std::string& arg) {

        std::string name;
        std::string val;
        bool state = false;

        for (auto c : arg) {

            if (::isspace(c)) {
                continue;

            } else if (c == ',') {
                state = true;

            } else {
                if (state) {
                    val += c;
                } else {
                    name += c;
                }
            }
        }

        const_map[name] = process_val(val);
    }

    void process_symbol(const std::string& arg) {

        std::string val;
        std::string n;
        enum {
            IN_START,
            IN_STRING,
            IN_QUOTE,
            IN_N
        } state = IN_START;

        for (auto c : arg) {
            switch (state) {

            case IN_START: {
                if (::isspace(c)) {
                    // Nothing
                } else if (c == '\'') {
                    state = IN_STRING;

                } else {
                    throw std::runtime_error("Invalid symtable directive: " + arg);
                }
                break;
            }

            case IN_STRING: {
                if (c == '\\') {
                    state = IN_QUOTE;
                } else if (c == '\'') {
                    state = IN_N;
                } else {
                    val += c;
                }
                break;
            }

            case IN_QUOTE: {
                if (c == 't') {
                    val += '\t';
                } else if (c == 'n') {
                    val += '\n';
                } else {
                    val += c;
                }
                state = IN_STRING;
                break;
            }

            case IN_N: {
                if (::isspace(c) || c == ',') {
                    // Nothing
                } else {
                    n += c;
                }
                break;
            }
            }
        }

        vm.symtab[process_uint(n)] = val;
    }

    void process_opcode(const std::string& opcode, const std::string& arg) {

        if (opcode.size() > 0 && opcode[0] == '.') {

            if (opcode == ".CONST") {
                process_const(arg);

            } else if (opcode == ".LABEL") {
                process_label(arg);

            } else if (opcode == ".SYMBOL") {
                process_symbol(arg);

            } else {
                throw std::runtime_error("Invalid directive: " + opcode);
            }
            return;
        }

        auto i = opcodes_map.opcodes_map.find(opcode);

        if (i == opcodes_map.opcodes_map.end())
            throw std::runtime_error("Unknown opcode: " + opcode);

        vm.code.push_back(Opcode());
        Opcode& opc = vm.code.back();

        opc.op = i->second.first;

        switch (i->second.second) {
        case VAL:
            opc.arg = process_val(arg);
            break;
        case UINT:
            opc.arg.uint = process_uint(arg);
            break;
        case ADDR:
            opc.arg.uint = process_addr(arg);
            break;
        }
    }

    void assemble(const std::string& s) {

        std::string::const_iterator i = s.begin();
        std::string opcode;
        std::string arg;

        enum {
            IN_BLANK,
            IN_OPCODE,
            IN_ARG
        } state = IN_BLANK;

        while (1) {

            if (i == s.end())
                break;

            switch (state) {

            case IN_BLANK: {
                if (::isspace(*i)) {
                    // Nothing.
                } else {
                    state = IN_OPCODE;
                    continue;
                }
                break;
            }

            case IN_OPCODE: {
                if (*i == '(') {
                    state = IN_ARG;

                } else if (::isspace(*i)) {
                    process_opcode(opcode, arg);
                    opcode.clear();
                    arg.clear();
                    state = IN_BLANK;

                } else {
                    if (::isalpha(*i)) {
                        opcode += ::toupper(*i);
                    } else {
                        opcode += *i;
                    }
                }
                break;
            }
             
            case IN_ARG: {
                if (*i == ')') {
                    process_opcode(opcode, arg);
                    opcode.clear();
                    arg.clear();
                    state = IN_BLANK;

                } else {
                    arg += *i;
                }
                break;
            }
            }

            ++i;
        }
    }
    
};


}

#endif

