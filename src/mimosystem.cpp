#include "mimosystem.hpp"
//------------------------------------------------------------------------------
#include "sigproc.hpp"
#include "signode.hpp"
#include <algorithm>
//------------------------------------------------------------------------------

using std::begin;
using std::end;
using std::find_if;

//------------------------------------------------------------------------------
mimosystem::mimosystem()
{
    sigprocs_.reserve(1024);
    signodes_.reserve(1024);
}
//------------------------------------------------------------------------------
void mimosystem::clock() const
{
    for (auto &sig : sigprocs_)
    {
        sig->clock();
    }
}
//------------------------------------------------------------------------------
void mimosystem::destroy(unsigned idx)
{
    if (idx >= sigprocs_.size())
       return;

    auto it = sigprocs_.begin() + idx;

    // Find its output nodes, unlink them from their associated sigprocs.
    // must be done in signodes_ and outnodes_.
    // ... TODO

    sigprocs_.erase(it);
}
//------------------------------------------------------------------------------
void mimosystem::destroy(sigproc *sproc)
{
    auto it = find_if(begin(sigprocs_), end(sigprocs_), 
                [sproc](decltype(sigprocs_[0]) &a)
                {
                    return a.get() == sproc;
                });
    return destroy(it - begin(sigprocs_));
}
//------------------------------------------------------------------------------
void mimosystem::addnodes(sigproc *sproc)
{
    // TODO
    const auto n = sproc->num_outnodes();
    for (unsigned i=0; i<n; ++i)
    {
        signodes_.push_back(sproc->outnode(i));
    }
}
//------------------------------------------------------------------------------

