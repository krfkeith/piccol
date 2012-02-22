#ifndef __CROMLECH_RUN
#define __CROMLECH_RUN

namespace crom {

template <typename T1, typename T2>
inline auto _do_op(T1 t1, T2 t2, int op) -> decltype(t1/t2) {
    switch (op){ 
    case ADD:
        return t1+t2;
    case SUB:
        return t1-t2;
    case MUL:
        return t1*t2;
    case DIV:
        return t1/t2;
    case MOD:
        throw std::runtime_error("TODO");
        //return t1%t2;
    }
}

inline void run(Vm& vm, const Val& arg, const std::vector<Opcall>& code) {

    Vm::iptr_t ip = code.begin();

    vm.runtime_frame.push_back(std::make_pair(arg, ip));

    while (1) {
        const Opcall& o = *ip;

        std::cout << "++ " << o.type << " " << vm.runtime_stack.size() << std::endl;
        
        switch (o.type) {
        case LITERAL:
            vm.runtime_stack.push_back(o.val);
            break;

        case FRAME_GET:
            vm.runtime_stack.push_back(vm.runtime_frame.back().first);
            break;

        case GET:
        {
            Val tmp = vm.runtime_stack.back();
            vm.runtime_stack.pop_back();

            Val::stup_t& vv = get<Val::stup_t>(tmp);
            vm.runtime_stack.push_back((*vv)[o.arg]);
            break;
        }

        case START_STRUCT:
            vm.runtime_stack.push_back(empty_struct());
            break;

        case PUSH_STRUCT:
        {
            Val v;
            v.swap(vm.runtime_stack.back());
            vm.runtime_stack.pop_back();

            get<Val::stup_t>(vm.runtime_stack.back())->push_back(Val());
            get<Val::stup_t>(vm.runtime_stack.back())->back().swap(v);
            break;
        }

        case FUNCALL:
        {
            auto tmp = vm.funs.equal_range(get<Symbol>(o.val));

            for (int i = o.arg; i > 0; --i) {
                tmp.first++;
            }

            vm.runtime_frame.push_back(std::make_pair(vm.runtime_stack.back(), ip));
            vm.runtime_stack.pop_back();
            ip = tmp.first->second.code.begin();
            continue;
            break;
        }

        case SYSCALL:
        {
            const Vm::syscall& tmp = vm.syscalls[get<Symbol>(o.val)];
            Val in = vm.runtime_stack.back();
            vm.runtime_stack.pop_back();
            vm.runtime_stack.push_back(tmp.cb(in));
            break;
        }


        case RETURN:
            ip = vm.runtime_frame.back().second;
            vm.runtime_frame.pop_back();

            if (vm.runtime_frame.empty())
                return;
            break;

        case CLEAR:
            vm.runtime_stack.clear();
            break;

        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case MOD:
        {
            Val v2 = vm.runtime_stack.back();
            vm.runtime_stack.pop_back();

            Val v1 = vm.runtime_stack.back();
            vm.runtime_stack.pop_back();

            switch (o.arg) {
            case 4:
                vm.runtime_stack.push_back(_do_op(get<Int>(v1), get<Int>(v2), o.type));
                break;
            case 3:
                vm.runtime_stack.push_back(_do_op(get<Int>(v1), get<Real>(v2), o.type));
                break;
            case 2:
                vm.runtime_stack.push_back(_do_op(get<Real>(v1), get<Int>(v2), o.type));
                break;
            case 1:
                vm.runtime_stack.push_back(_do_op(get<Real>(v1), get<Real>(v2), o.type));
                break;
            }
            break;
        }
        }

        ++ip;
    }
}

inline void run(Vm& vm, const Val& arg, Symbol fun) {

    if (vm.funs.count(fun) != 1) {
        throw std::runtime_error("Could not run Vm: invalid callback: " + symstr(fun));
    }

    const auto tmp = vm.funs.equal_range(fun);

    for (auto i = tmp.first; i != tmp.second; ++i) {
        run(vm, arg, i->second.code);
    }
}


}


#endif

