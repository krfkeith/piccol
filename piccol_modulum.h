#ifndef __PICCOL_MOD_H
#define __PICCOL_MOD_H

/*
 * This is an optional add-on for Basic Piccol which adds support for
 * modular programming. (modules)
 *
 * Also included is support for global variables and a basic key-value storage.
 *
 */

#include "piccol_vm.h"

#include <memory>
#include <unordered_set>



namespace piccol {

struct PiccolF : public Piccol {

    std::string appdir;

    PiccolF(const PiccolF& p) : Piccol(static_cast<const Piccol&>(p)), appdir(p.appdir) {}

    PiccolF(PiccolF&& p) : Piccol(static_cast<const Piccol&&>(p)), appdir(p.appdir) {}

    PiccolF(const std::string& sysdir, const std::string& ad, bool _verbose = false) : 
        Piccol(piccol::load_file(sysdir + "macrolan.metal"),
               piccol::load_file(sysdir + "piccol_lex.metal"),
               piccol::load_file(sysdir + "piccol_morph.metal"),
               piccol::load_file(sysdir + "piccol_emit.metal"),
               piccol::load_file(sysdir + "prelude.piccol"),
               _verbose),
        appdir(ad)
        {
            Piccol::init();
        }

    void load(const std::string& fn) {
        try {
            Piccol::load(piccol::load_file(appdir + fn));
        } catch (std::exception& e) {
            std::string msg = ("In module: " + fn + ": " + e.what());
            throw std::runtime_error(msg);
        }
    }
};

namespace {

bool _route(PiccolF& vm, const label_t& l, 
            const Shapes& shapes, const Shape& shape, 
            const Shape& shapeto, const Struct& struc, Struct& ret) {

    return vm.run(l.name, l.fromshape, l.toshape, struc, ret);
}

}

using namespace std::placeholders;

struct Modules {

    // modulename -> filename, vm
    std::unordered_map< Sym, std::pair<Sym, std::shared_ptr<PiccolF> > > modules;

    // funcname -> modulename
    std::unordered_map<label_t,Sym> exports;

    // callbacks that are common to all modules.
    std::unordered_map<label_t, callback_t> callbacks;

    // functions that are required to be exported
    std::unordered_set<label_t> _required;

    std::string sysdir;
    std::string appdir;

    std::string common;

    bool verbose;

    Modules(const std::string& sd, 
            const std::string& ad,
            const std::string& loader, 
            bool _verbose = false) : sysdir(sd), appdir(ad), verbose(_verbose) {

        PiccolF p(sysdir, appdir, verbose);

        p.register_callback("module", "[ Sym Sym ]", "Void", 
                            std::bind(&Modules::_cb_module, this, _1, _2, _3, _4, _5));

        p.register_callback("exported", "[ Sym Sym Sym Sym ]", "Void",
                            std::bind(&Modules::_cb_exported, this, _1, _2, _3, _4, _5));

        p.register_callback("common", "Sym", "Void",
                            std::bind(&Modules::_cb_common, this, _1, _2, _3, _4, _5));

        // Warning, ugly hard-coded metaprogramming follows.

        static_cast<Piccol&>(p).load("def [module:Sym funname:Sym funintype:Sym funouttype:Sym]; "
                                     "<:: module(type_canonical,literals) "
                                     "modname :- ident &'push'. "
                                     "ident_here :- ident &''. "
                                     "funs :- spaces @{['} &'top' @{' '} ident_here @{' '}  "
                                     "        spaces type_canonical @{' '} spaces '->' "
                                     "        spaces type_canonical @{'] exported } funs. "
                                     "funs :- . "
                                     "sym_here :- sym &''. "
                                     "module :- spaces modname spaces @{['} &'top' @{' } sym_here @{] module } "
                                     "          funs."
                                     "::>");

        p.load(loader);

        Struct tmp;
        p.run("modules", "Void", "Void", tmp);
    }

    bool _cb_module(const Shapes& shapes, const Shape& shape, 
                    const Shape& shapeto, const Struct& struc, Struct& ret) {

        Sym module = struc.v[0].uint;
        Sym filename = struc.v[1].uint;

        auto i = modules.find(module);

        if (i != modules.end()) {
            throw std::runtime_error("Tried to define a module twice: '" + metalan::symtab().get(module) + "'");
        }

        modules[module] = std::make_pair(filename, std::shared_ptr<PiccolF>());

        return true;
    }

    bool _cb_exported(const Shapes& shapes, const Shape& shape, 
                      const Shape& shapeto, const Struct& struc, Struct& ret) {

        Sym module = struc.v[0].uint;
        Sym fnname = struc.v[1].uint;
        Sym fnfrom = struc.v[2].uint;
        Sym fnto = struc.v[3].uint;

        label_t l(fnname, fnfrom, fnto);

        auto i = exports.find(l);

        if (i != exports.end()) {
            throw std::runtime_error("Function exported twice: " + l.print());
        }

        if (modules.count(module) == 0) {
            throw std::runtime_error("Function " + l.print() + " exported from unknown module: '" +
                                     metalan::symtab().get(module) + "'");
        }

        exports[l] = module;
        return true;
    }

    bool _cb_common(const Shapes& shapes, const Shape& shape, 
                    const Shape& shapeto, const Struct& struc, Struct& ret) {

        if (common.size() != 0) {
            throw std::runtime_error("Only one 'common' section is allowed in a module spec");
        }

        common = metalan::symtab().get(struc.v[0].uint);
        return true;
    }


    void _link() {

        // Check prerequisites
        for (const auto& r : _required) {
            if (exports.count(r) == 0) {
                throw std::runtime_error("Function " + r.print() + " is required but not exported by any module");
            }
        }

        // Load common code.
        PiccolF comvm(sysdir, appdir, verbose);

        if (common.size() != 0) {
            comvm.load(common);
        }

        // Export foreign functions in all other VMs.

        for (auto& mod : modules) {

            mod.second.second = std::shared_ptr<PiccolF>(new PiccolF(comvm));
        }

        for (auto& mod : modules) {
            PiccolF& vm = *(mod.second.second);

            for (const auto& cb : callbacks) {
                vm.code.register_callback(cb.first, cb.second);
            }

            for (const auto& func : exports) {

                if (func.second == mod.first) 
                    continue;

                PiccolF& othervm = *(modules[func.second].second);

                vm.code.register_callback(func.first, 
                                          std::bind(_route, std::ref(othervm), std::cref(func.first),
                                                    _1, _2, _3, _4, _5));
            }
        }

        // Load the module sources and check that all exports are defined.

        for (auto& mod : modules) {

            PiccolF& vm = *(mod.second.second);

            vm.load(metalan::symtab().get(mod.second.first));

            for (const auto& func : exports) {
                
                if (func.second != mod.first)
                    continue;

                if (vm.code.codes.count(func.first) == 0) {
                    
                    throw std::runtime_error("Function " + func.first.print() + " is exported but not defined");
                }
            }
        }
    }


    // Public interface follows.

    const Shape& get_type(const std::string& shape) {
        // Very much HACK.
        const Shape* ret = NULL;

        for (const auto& mod : modules) {
            try {
                ret = &(mod.second.second->get_type(shape));

            } catch (std::exception& e) {
                throw std::runtime_error("In module " + metalan::symtab().get(mod.second.first) + ": " +
                                         e.what());
            }
        }

        return *ret;
    }

    void check_type(const std::string& shape, std::initializer_list<nanom::Type> types) {

        for (const auto& mod : modules) {
            try {
                mod.second.second->check_type(shape, types);
            } catch (std::exception& e) {
                throw std::runtime_error("In module " + metalan::symtab().get(mod.second.first) + ": " +
                                         e.what());
            }
        }
    }

    void init() {
        _link();
    }

    void required(const std::string& name, const std::string& from, const std::string& to) {
        label_t l(metalan::symtab().get(name),
                  metalan::symtab().get(from), 
                  metalan::symtab().get(to));
        _required.insert(l);
    }

    void register_callback(const std::string& name, const std::string& from, const std::string& to,
                           callback_t cb) {

        label_t l(metalan::symtab().get(name),
                  metalan::symtab().get(from), 
                  metalan::symtab().get(to));

        if (callbacks.find(l) != callbacks.end()) {
            throw std::runtime_error("Callback registered twice: " + l.print());
        }

        callbacks[l] = cb;
    }


    bool run(const std::string& name, const std::string& fr, const std::string& to, 
             const Struct& in, Struct& out) {

        label_t l(metalan::symtab().get(name), metalan::symtab().get(fr), metalan::symtab().get(to));

        auto i = exports.find(l);

        if (i == exports.end()) {
            throw std::runtime_error("Unknown exported function called: " + l.print());
        }

        PiccolF& othervm = *(modules[i->second].second);

        if (othervm.verbose) {
            std::cout << "=== " << l.print() << " ===" << std::endl;
        }
        
        return othervm.run(l.name, l.fromshape, l.toshape, in, out);
    }
};

}


#endif
