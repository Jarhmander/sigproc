#ifndef SIGNODE_HPP
#define SIGNODE_HPP
//------------------------------------------------------------------------------
#include "common.hpp"
#include <string>
//------------------------------------------------------------------------------

namespace dspunit
{

class sigproc;

class signode
{
public:
    signode(sigproc *sproc);

    // Update is called when a sigproc want to read its input(s) signode(s)
    sigvalue_t update(clockref_t inclock);

    // The sigproc driving a particular signode may call commit on it to
    // establish the new node value
    void commit(clockref_t inclock, sigvalue_t value);

    sigproc *proc() const { return sigproc_; }
    bool value(clockref_t inclock, sigvalue_t &v) const;
    sigvalue_t value() const { return value_; }

    std::string name;

    static signode *nullnode();
private:
    sigproc *const sigproc_;
    clockref_t clockref_    = -1u;
    sigvalue_t value_       = {};
};

} // namespace dspunit

//------------------------------------------------------------------------------
#endif // SIGNODE_HPP
