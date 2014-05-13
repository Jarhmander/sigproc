#ifndef VECSET_HPP
#define VECSET_HPP
//------------------------------------------------------------------------------
#include <vector>
#include <algorithm>
//------------------------------------------------------------------------------

namespace dspunit
{


template <typename Container, typename Key> 
 Container &insert_sort(Container &c, Key &&key)
{
    using std::lower_bound;
    using std::begin;
    using std::end;
    using std::move;

    auto endc = end(c);
    auto it = lower_bound(begin(c), endc, key);
    if (it == endc || key < *it)
        c.emplace(it, move(key));

    return c;
}

template <typename Container, typename Key, typename Comp> 
 Container &insert_sort(Container &c, Key &&key, Comp p)
{
    using std::lower_bound;
    using std::begin;
    using std::end;
    using std::move;

    auto endc = end(c);
    auto it = lower_bound(begin(c), endc, key, p);
    if (it == endc || p(key, *it))
        c.emplace(it, move(key));

    return c;
}

template <typename Container, typename InputIt>
 Container &insert_sort_range(Container &c, InputIt first, InputIt last)
{
    for (; first != last; ++first)
    {
        insert_sort(c, *first);
    }
    return c;
}

template <typename Container, typename InputIt, typename Comp>
 Container &insert_sort_range(Container &c, InputIt first, InputIt last, Comp p)
{
    Comp &cp = p;
    for (; first != last; ++first)
    {
        insert_sort(c, *first, cp);
    }
    return c;
}

template <typename Container, typename Key>
 auto find(Container &c, const Key &key) -> decltype(c.begin())
{
    using std::lower_bound;
    using std::begin;
    using std::end;

    auto endc = end(c);
    auto it = lower_bound(begin(c), endc, key);
    if (it == endc || (key < *it))
        return endc;
    return it;
}

template <typename Container, typename Key, typename Comp>
 auto find(Container &c, const Key &key, Comp p) -> decltype(c.begin())
{
    using std::lower_bound;
    using std::begin;
    using std::end;

    auto endc = end(c);
    Comp &cp = p;
    auto it = lower_bound(begin(c), endc, key, cp);
    if (it == endc || p(key, *it))
        return endc;
    return it;
}

template <typename Container, typename Key>
 Key find_or(Container &c, const Key &key, Key &&defkey = {}) 
{
    using std::begin;
    using std::end;
    
    auto it = find(c, key);
    return it == end(c) ? defkey : *it;
}

template <typename Container, typename Key, typename Comp>
 Key find_or(Container &c, const Key &key, Key &&defkey, Comp p)
{
    using std::begin;
    using std::end;
    
    Comp &cp = p;
    auto it = find(c, key, p);
    return it == end(c) ? defkey : *it;
}

} // namespace dspunit

//------------------------------------------------------------------------------
#endif // VECSET_HPP

