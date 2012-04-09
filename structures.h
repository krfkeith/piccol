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

namespace {

int find_midpoint(int midpoint, const Shape& s) {

    if (midpoint >= 0) return midpoint;

    auto it = shape.sym2field.begin();
    const Shape::typeinfo& ti1 = it->second;
    ++it;
    const Shape::typeinfo& ti2 = it->second;

    if (ti1.ix_to == ti2.ix_from) {
        midpoint = ti1.ix_to;
    } else {
        midpoint = ti2.ix_to;
    }
}

}

struct StructMap {
    
    typedef std::unordered_map<Struct, Struct> map_t;
    
    map_t map;
    int midpoint;

    StructMap() : midpoint(-1) {}

    bool set(const Struct& k, const Struct& v) { 
        auto i = map.find(k);
        if (i != map.end())
            return false;
        map[k] = v;
        return true;
    }

    bool set(const Shape& shape, const Struct& kv) {

        midpoint = find_midpoint(midpoint, shape);

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
    int midpoint;

    StructPool() : midpoint(-1) {}

    bool add(const Struct& k, const Struct& v) {
        auto& i = map[k];
        auto& j = i[v];
        ++(j->second);
        return true;
    }

    bool add(const Shape& shape, const Struct& kv) {
        
        midpoint = find_midpoint(midpoint, shape);

        return add(kv.substruct(0, midpoint), kv.substruct(midpoint, shape.size()));
    }

    bool get(const Struct& k, Struct& v, size_t n) {
        auto i = map.find(k);
        if (i == map.end()) {
            return false;
        }

        size_t q = 0;
        for (const auto& j : i->second) {
            if (q >= n && q < n + j.second) {
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


}

#endif
