#ifndef __PICCOL_STRUCTURES_H
#define __PICCOL_STRUCTURES_H

#include "piccol_vm.h"

namespace std {

template <>
struct hash<nanom::Struct> {
    size_t operator()(const nanom::Struct& v) const {
        size_t h = 0;
        for (const auto& i : v.v) {
            h += hash<nanom::UInt>()(i.uint);
        }
        return hash<size_t>()(h);
    }
};

template <>
struct equal_to<nanom::Struct> {

    bool operator()(const nanom::Struct& a, const nanom::Struct& b) const {
        auto ai = a.v.begin();
        auto bi = b.v.begin();
        auto ae = a.v.end();
        auto be = b.v.end();

        while (1) {
            if (ai == ae && bi == be) return true;
            if (ai == ae || bi == be) return false;
            if (ai->uint != bi->uint) return false;
            ++ai;
            ++bi;
        }
    }
};

}


namespace piccol {


struct StructMap {
    
    typedef std::unordered_map<Struct, Struct> map_t;
    
    map_t map;

    StructMap() {}

    bool set(const Struct& k, const Struct& v) { 
        auto i = map.find(k);
        if (i != map.end())
            return false;
        map[k] = v;
        return true;
    }

    bool set(const Shape& shape, const Struct& kv) {

        size_t midpoint = shape.get_nth_type(0).ix_to;

        return set(kv.substruct(0, midpoint), kv.substruct(midpoint, shape.size()));
    }

    bool get(const Struct& k, Struct& v) const {
        auto i = map.find(k);
        if (i == map.end())
            return false;
        v = i->second;
        return true;
    }

    bool del(const Struct& k, Struct& v) {
        auto i = map.find(k);
        if (i == map.end())
            return false;
        v = i->second;
        map.erase(i);
        return true;
    }

    void clear() {
        map.clear();
    }
};


struct StructPool {

    typedef std::unordered_map< Struct, std::unordered_map<Struct, size_t> > map_t;

    map_t map;

    StructPool() {}

    // Insert N objects of V with key K to the pool.

    bool put(const Struct& k, const Struct& v, size_t n) {

        auto& i = map[k];
        auto& j = i[v];
        j += n;
        return true;
    }

    bool put(const Shape& shape, const Struct& kvn) {
        
        size_t midpoint = shape.get_nth_type(0).ix_to;

        return put(kvn.substruct(0, midpoint), 
                   kvn.substruct(midpoint, shape.size()-1),
                   kvn.v.back().uint);
    }

    // Get the Nth object with key K -> V.

    bool get(const Struct& k, Struct& v, size_t n) {
        auto i = map.find(k);
        if (i == map.end()) {
            return false;
        }

        size_t q = 0;
        for (auto& j : i->second) {
            if (n >= q && n < q + j.second) {
                v = j.first;
                j.second--;

                if (j.second == 0) {
                    i->second.erase(j.first);

                    if (i->second.empty()) {
                        map.erase(i);
                    }
                }

                return true;
            }
            q += j.second;
        }

        return false;
    }

    // Check that object V exists with key K, decrement its counter.

    bool get(const Struct& k, const Struct& v) {

        auto i = map.find(k);
        if (i == map.end()) {
            return false;
        }

        auto j = i->second.find(v);
        if (j == i->second.end()) {
            return false;
        }

        --(j->second);

        if (j->second == 0) {
            i->second.erase(j->first);

            if (i->second.empty()) {
                map.erase(i);
            }
        }

        return true;
    }

    // [ K N ] -> V
    
    bool get(const Shape& shape, const Struct& kn, Struct& v) {

        return get(kn.substruct(0, shape.size()-1), v, kn.v.back().uint);
    }

    // [ K V ] -> Void

    bool get(const Shape& shape, const Struct& kv) {

        size_t midpoint = shape.get_nth_type(0).ix_to;

        return get(kv.substruct(0, midpoint), kv.substruct(midpoint, shape.size()));
    }

    // Return the size of pool with key K.

    bool size(const Struct& k, size_t& v) {
        auto i = map.find(k);
        if (i == map.end()) {
            return false;
        }

        size_t q = 0;
        for (const auto& j : i->second) {
            q += j.second;
        }

        v = q;
        return true;
    }

    bool size(const Struct& k, Struct& n) {
        
        size_t tmp;
        bool ret = size(k, tmp);

        if (ret) {
            n.v.push_back((nanom::UInt)tmp);
        }

        return ret;
    }
};


struct GlobalStruct {

    Struct obj;
    bool init;

    GlobalStruct() : init(false) {}

    bool set(const Struct& v) {
        obj = v;
        init = true;
        return true;
    }

    bool get(Struct& v) const {
        if (!init) return false;
        v = obj;
        return true;
    }

    bool del(Struct& v) {
        if (!init) return false;
        init = false;
        v = obj;
        return true;
    }
};


template <typename T>
inline StructMap& structmap() {
    static StructMap ret;
    return ret;
}

template <typename T>
inline GlobalStruct& globalstruct() {
    static GlobalStruct ret;
    return ret;
}

template <typename T>
inline StructPool& structpool() {
    static StructPool ret;
    return ret;
}


template <typename T>
inline bool structmap_set(const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                          const Struct& struc, Struct& ret) {

    return structmap<T>().set(shape, struc);
}

template <typename T>
inline bool structmap_get(const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                          const Struct& struc, Struct& ret) {
    return structmap<T>().get(struc, ret);
}

template <typename T>
inline bool structmap_del(const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                          const Struct& struc, Struct& ret) {
    return structmap<T>().del(struc, ret);
}


template <typename T>
inline bool globalstruct_set(const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                             const Struct& struc, Struct& ret) {

    return globalstruct<T>().set(struc);
}

template <typename T>
inline bool globalstruct_get(const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                             const Struct& struc, Struct& ret) {
    return globalstruct<T>().get(ret);
}

template <typename T>
inline bool globalstruct_del(const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                             const Struct& struc, Struct& ret) {
    return globalstruct<T>().del(ret);
}


template <typename T>
inline bool structpool_put(const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                           const Struct& struc, Struct& ret) {

    return structpool<T>().put(shape, struc);
}

template <typename T>
inline bool structpool_get_n(const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                           const Struct& struc, Struct& ret) {
    return structpool<T>().get(shape, struc, ret);
}

template <typename T>
inline bool structpool_get_k(const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                           const Struct& struc, Struct& ret) {
    return structpool<T>().get(shape, struc);
}

template <typename T>
inline bool structpool_size(const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                            const Struct& struc, Struct& ret) {
    return structpool<T>().size(struc, ret);
}


template <typename MAP, typename VM>
void register_map(VM& vm, const std::string& shapefrom, const std::string& shapeto) {

    std::string twoshapes = "[ " + shapefrom + " " + shapeto + " ]";

    vm.register_callback("set", twoshapes, "Void", structmap_set<MAP>);
    vm.register_callback("get", shapefrom, shapeto, structmap_get<MAP>);
    vm.register_callback("del", shapefrom, shapeto, structmap_del<MAP>);
}


template <typename GLOBJ, typename VM>
void register_global(VM& vm, const std::string& shape) {

    vm.register_callback("set", shape,  "Void", globalstruct_set<GLOBJ>);
    vm.register_callback("get", "Void", shape,  globalstruct_get<GLOBJ>);
    vm.register_callback("del", "Void", shape,  globalstruct_del<GLOBJ>);
}


template <typename POOL, typename VM>
void register_pool(VM& vm, const std::string& shapefrom, const std::string& shapeto) {

    std::string thrshapes = "[ " + shapefrom + " " + shapeto + " UInt ]";
    std::string nshapes = "[ " + shapefrom + " UInt ]";
    std::string kshapes = "[ " + shapefrom + " " + shapeto + " ]";

    vm.register_callback("put",  thrshapes, "Void",  structpool_put<POOL>);
    vm.register_callback("get",  nshapes,   shapeto, structpool_get_n<POOL>);
    vm.register_callback("get",  kshapes,   "Void",  structpool_get_k<POOL>);
    vm.register_callback("size", shapefrom, "UInt",  structpool_size<POOL>);
}


}

#endif
