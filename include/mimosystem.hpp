#ifndef MIMOSYSTEM_HPP
#define MIMOSYSTEM_HPP
//------------------------------------------------------------------------------
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <memory>
//------------------------------------------------------------------------------

class sigproc;

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
    template <typename Key, typename HashF>
                             using set   = std::unordered_set<Key,HashF>;

    vec<ptr<sigproc>>                               sigprocs_;
    hasht<signode*,set<input_desc, indesc_hash>>    connections_;
    vec<signode *>                                  outnodes_;

    unsigned current_clock_ = 0;

public:
    mimosystem();

    auto out(bool postclock = true) -> const vec<signode *> &;

    void  clock() const;

    void current_clock(unsigned newclk) { current_clock_ = newclk; }

    unsigned current_clock() const      { return current_clock_; }

    template <typename T, typename ...Args>
     T *create(Args&&... args)
    {
        auto p   = ptr<T>(new T(std::forward<Args>(args)...));
        auto ret = p.get();
        newsigproc(std::move(p));
        return ret;
    }

    void destroy(unsigned idx);
    void destroy(sigproc *);

    unsigned index(sigproc *) const;

    sigproc *connect(sigproc *, unsigned in, signode *);
    sigproc *connect(unsigned idx, unsigned in, signode *node);

    sigproc *disconnect(sigproc *, unsigned in);
    sigproc *disconnect(unsigned idx, unsigned in);

    signode *link(signode *, unsigned out);
    signode *linknext(signode *);
    signode *unlink(unsigned out);

    auto procs() const -> const decltype(sigprocs_) &
    {
        return sigprocs_;
    }

private:
    void newsigproc(ptr<sigproc> sproc);
};

//------------------------------------------------------------------------------
#endif // MIMOSYSTEM_HPP
