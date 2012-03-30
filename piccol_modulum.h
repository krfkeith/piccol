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


namespace piccol {

struct PiccolF : public Piccol {

    PiccolF(const std::string& sysdir = "") :
        Piccol(piccol::load_file(sysdir + "macrolan.metal"),
               piccol::load_file(sysdir + "piccol_lex.metal"),
               piccol::load_file(sysdir + "piccol_morph.metal"),
               piccol::load_file(sysdir + "piccol_emit.metal"),
               piccol::load_file(sysdir + "prelude.piccol"))
        {
            Piccol::init();
        }

    void load(const std::string& fn) {
        Piccol::load(piccol::load_file(fn));
    }
};

namespace {

bool _route(PiccolF& vm, const nanom::label_t& l, 
            const nanom::Shapes& shapes, const nanom::Shape& shape, 
            const nanom::Shape& shapeto, const nanom::Struct& struc, nanom::Struct& ret) {

    return vm.run(l.name, l.fromshape, l.toshape, struc, ret);
}

}

using namespace std::placeholders;

struct Modules {

    // modulename -> filename, modules_d index
    std::unordered_map< Sym, std::pair<Sym, std::shared_ptr<PiccolF> > > modules;

    // funcname -> modulename
    std::unordered_map<nanom::label_t,Sym> exports;

    // callbacks that are common to all modules.
    std::unordered_map<label_t, callback_t> callbacks;

    std::string sysdir;
    std::string appdir;

    Modules(const std::string& sd, 
            const std::string& ad,
            const std::string& loader) : sysdir(sd), appdir(ad) {

        PiccolF p(sysdir);

        _register_system_callbacks(p);
        static_cast<Piccol&>(p).load("def [module:Sym funname:Sym funintype:Sym funouttype:Sym];");
        p.load(appdir + loader);

        nanom::Struct tmp;
        p.run("modules", "Void", "Void", tmp);
    }

    void _register_system_callbacks(PiccolF& p) {

        p.register_callback("module", "[ Sym Sym ]", "Void", 
                            std::bind(&Modules::_cb_module, this, _1, _2, _3, _4, _5));

        p.register_callback("exported", "[ Sym Sym Sym Sym ]", "Void",
                            std::bind(&Modules::_cb_exported, this, _1, _2, _3, _4, _5));
    }

    bool _cb_module(const nanom::Shapes& shapes, const nanom::Shape& shape, 
                    const nanom::Shape& shapeto, const nanom::Struct& struc, nanom::Struct& ret) {

        nanom::Sym module = struc.v[0].uint;
        nanom::Sym filename = struc.v[1].uint;

        auto i = modules.find(module);

        if (i != modules.end()) {
            throw std::runtime_error("Tried to define a module twice: '" + metalan::symtab().get(module) + "'");
        }

        modules[module] = std::make_pair(filename, std::shared_ptr<PiccolF>(new PiccolF(sysdir)));
        return true;
    }

    bool _cb_exported(const nanom::Shapes& shapes, const nanom::Shape& shape, 
                      const nanom::Shape& shapeto, const nanom::Struct& struc, nanom::Struct& ret) {

        nanom::Sym module = struc.v[0].uint;
        nanom::Sym fnname = struc.v[1].uint;
        nanom::Sym fnfrom = struc.v[2].uint;
        nanom::Sym fnto = struc.v[3].uint;

        nanom::label_t l(fnname, fnfrom, fnto);

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

    void _link() {

        // Export foreign functions in all other VMs.

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

        // Load the module source and check that all exports are defined.

        for (auto& mod : modules) {

            PiccolF& vm = *(mod.second.second);

            vm.load(appdir + metalan::symtab().get(mod.second.first));

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

    void init() {
        _link();
    }

    void register_callback(const std::string& name, const std::string& from, const std::string& to,
                           nanom::callback_t cb) {

        nanom::label_t l(metalan::symtab().get(name),
                         metalan::symtab().get(from), 
                         metalan::symtab().get(to));

        if (callbacks.find(l) != callbacks.end()) {
            throw std::runtime_error("Callback registered twice: " + l.print());
        }

        callbacks[l] = cb;
    }


    bool run(const std::string& name, const std::string& fr, const std::string& to, 
             const nanom::Struct& in, nanom::Struct& out) {

        nanom::label_t l(metalan::symtab().get(name), metalan::symtab().get(fr), metalan::symtab().get(to));

        auto i = exports.find(l);

        if (i == exports.end()) {
            throw std::runtime_error("Unknown exported function called: " + l.print());
        }

        PiccolF& othervm = *(modules[i->second].second);
        
        return othervm.run(l.name, l.fromshape, l.toshape, in, out);
    }
};

}


#endif
