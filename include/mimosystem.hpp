#ifndef MIMOSYSTEM_HPP
#define MIMOSYSTEM_HPP
//------------------------------------------------------------------------------
#include "common.hpp"
//------------------------------------------------------------------------------
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <memory>
//------------------------------------------------------------------------------

namespace dspunit
{

class sigproc;

class sigproclkd;

class signode;

class mimosystem
{
private:
    struct input_desc
    {
        sigproc *sproc;
        unsigned input;
        bool operator==(const input_desc &other) const
        {
            return this->sproc == other.sproc
                && this->input == other.input;
        }
    };

    struct indesc_hash
    {
        std::size_t operator()(const input_desc &id) const
        {
            return std::hash<void*>()(id.sproc) 
                 ^ std::hash<unsigned>()(id.input);
        }
    };

    template <typename T>   using ptr   = std::unique_ptr<T>;
    template <typename T>   using vec   = std::vector<T>;
    template <typename Key, typename T> 
                            using hasht = std::unordered_map<Key,T>;
    template <typename Key, typename HashF=std::hash<Key>>
                             using set   = std::unordered_set<Key,HashF>;

    vec<ptr<sigproc>>                               sigprocs_;
    hasht<signode*,set<input_desc, indesc_hash>>    connections_;
    set<sigproclkd *>                               sigproclkds_;
    vec<signode *>                                  outnodes_;

    clockref_t current_clock_ = 0;

public:
    mimosystem();

    const vec<signode *> &out()
    {
        return outnodes_;
    }

    const vec<signode *> &update();

    const vec<signode *> &update(clockref_t newclk)
    {
        current_clock_ = newclk;
        return update();
    }


    void current_clock(clockref_t newclk) { current_clock_ = newclk; }

    clockref_t current_clock() const      { return current_clock_; }

    template <typename T, typename... Args>
     T *create(Args &&...args)
    {
        return create_internal( new T(std::forward<Args>(args)...) );
    }

    template <typename Func, typename... Args>
     auto createfrom(Func func, Args &&...args)
        -> decltype( func(std::forward<Args>(args)...) )
    {
        return create_internal( func(std::forward<Args>(args)...) );
    }

    void destroy(sigproc *);
    sigproc *connect(sigproc *, unsigned in, signode *);
    sigproc *disconnect(sigproc *, unsigned in);

    signode *link(signode *, unsigned out);
    signode *linknext(signode *);
    signode *unlink(unsigned out);
    void unlinkall();

    auto procs() const -> const decltype(sigprocs_) &
    {
        return sigprocs_;
    }

private:
    unsigned index(sigproc *) const;

    sigproc *connect(unsigned idx, unsigned in, signode *node);
    sigproc *disconnect(unsigned idx, unsigned in);
    void destroy(unsigned idx);

    template <typename T>
     T *create_internal(T *t)
    {
        auto p = ptr<T> { t };
        newsigproc(std::move(p));
        return t;
    }

    void newsigproc(ptr<sigproc> sproc);

    void updateout();

    void clock();
};

} // namespace dspunit

//------------------------------------------------------------------------------
#endif // MIMOSYSTEM_HPP
