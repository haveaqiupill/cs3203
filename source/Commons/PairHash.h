#pragma once

#include <utility>

struct pair_hash {
    /*
        Implemented a la boost::hash
        References:
        http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1837.pdf
        https://www.boost.org/doc/libs/1_74_0/boost/container_hash/hash.hpp
    */
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const {
        size_t seed = 0;
        seed ^= std::hash<T1>{}(p.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= std::hash<T2>{}(p.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};
