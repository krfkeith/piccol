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

    bool add(const Struct& k, const Struct& v) { 
        auto i = map.find(k);
        if (i != map.end())
            return false;
        map[k] = v;
        return true;
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
};


template <typename T>
inline StructMap& structmap() {
    static StructMap ret;
    return ret;
}

template <typename T>
inline bool structmap_set(const Shapes& shapes, const Shape& shape, const Shape& shapeto, 
                          const Struct& struc, Struct& ret,
                          size_t midpoint) {

    return structmap<T>().add(struc.substruct(0, midpoint), struc.substruct(midpoint, shape.size()));
    return true;
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

template <typename MAP, typename VM>
void register_map(VM& vm, const std::string& shapefrom, const std::string& shapeto) {

    std::string twoshapes = "[ " + shapefrom + " " + shapeto + " ]";
    const Shape& ts = vm.get_type(twoshapes);

    auto it = ts.sym2field.begin();
    const Shape::typeinfo& ti1 = it->second;
    ++it;
    const Shape::typeinfo& ti2 = it->second;

    size_t midpoint;
    if (ti1.ix_to == ti2.ix_from) {
        midpoint = ti1.ix_to;
    } else {
        midpoint = ti2.ix_to;
    }

    vm.register_callback("set", twoshapes, "Void", std::bind(structmap_set<MAP>, _1, _2, _3, _4, _5, midpoint));
    vm.register_callback("get", shapefrom, shapeto, structmap_get<MAP>);
    vm.register_callback("del", shapefrom, shapeto, structmap_del<MAP>);
}

}

#endif
