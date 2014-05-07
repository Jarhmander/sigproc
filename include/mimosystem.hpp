#ifndef MIMOSYSTEM_HPP
#define MIMOSYSTEM_HPP
//------------------------------------------------------------------------------
#include <vector>
#include <utility>
#include <memory>
//------------------------------------------------------------------------------

class sigproc;

class signode;

class mimosystem
{
private:
    template <typename T> using ptr = std::unique_ptr<T>;
    template <typename T> using vec = std::vector<T>;

    vec<ptr<sigproc>> sigprocs_;
    vec<signode *>    signodes_;
    vec<signode *>    outnodes_;

    unsigned current_clock = 0;
public:
    mimosystem();

    auto out() -> const vec<signode *> &
    {
        return outnodes_;
    }

    void  clock() const;

    template <typename T, typename ...Args>
     T *create(Args&&... args)
    {
        auto p   = ptr<T>(new T(std::forward<Args>(args)...));
        auto ret = p.get();
        sigprocs_.emplace_back(std::move(p));
        addnodes(ret);
        return ret;
    }
    void destroy(unsigned);
    void destroy(sigproc *);

    
private:
    void addnodes(sigproc *sproc);
};

//------------------------------------------------------------------------------
#endif // MIMOSYSTEM_HPP
