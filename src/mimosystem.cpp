#include "mimosystem.hpp"
//------------------------------------------------------------------------------
#include "sigproc.hpp"
#include "signode.hpp"
#include <algorithm>
//------------------------------------------------------------------------------

namespace dspunit
{

using std::begin;
using std::end;
using std::find;
using std::move;
using std::lower_bound;
using std::upper_bound;

//------------------------------------------------------------------------------
mimosystem::mimosystem()
{
    sigprocs_.reserve(32);
    connections_.reserve(32);
    sigproclkds_.reserve(32);
}
//------------------------------------------------------------------------------
auto mimosystem::out(bool postclock) -> const vec<signode *> &
{
    for (auto &e : outnodes_)
    {
        e->update(current_clock_);
    }
    if (postclock)
    {
        ++current_clock_;
        clock();
    }

    return outnodes_;
}
//------------------------------------------------------------------------------
void mimosystem::clock() const
{
    for (auto &sclk : sigproclkds_)
    {
        sclk->clock();
    }
}
//------------------------------------------------------------------------------
void mimosystem::destroy(unsigned idx)
{
    if (idx >= sigprocs_.size())
       return;

    auto it = sigprocs_.begin() + idx;
    const auto p = it->get();

    // Remove from sigproclks, if it exists
    sigproclkds_.erase(dynamic_cast<sigproclkd*>(p));

    // Find its output nodes, unlink them from their associated sigprocs.
    const auto numout = p->num_outnodes();

    for (unsigned i = 0; i<numout; ++i)
    {
        const auto deadnode = p->outnode(i);
        auto map_it = connections_.find(deadnode);
        if (map_it != end(connections_))
        {
            for (auto &e : map_it->second)
            {
                if (e.sproc)
                {
                    e.sproc->disconnect(e.input);
                }
            }
            connections_.erase(deadnode);
        }

        // Remove from outnodes_ if it exists
        auto begoutn = begin(outnodes_);
        auto endoutn = end(outnodes_);

        auto out_it = find(begoutn, endoutn, deadnode);
        if (out_it != endoutn)
        {
            *out_it = signode::nullnode();
        }
    }

    // Finally, remove from sigprocs_
    sigprocs_.erase(it);
}
//------------------------------------------------------------------------------
void mimosystem::destroy(sigproc *sproc)
{
    return destroy(index(sproc));
}
//------------------------------------------------------------------------------
unsigned mimosystem::index(sigproc *sproc) const
{
    auto begsigp = begin(sigprocs_);
    auto endsigp = end(sigprocs_);

    auto it = lower_bound(begsigp, endsigp, sproc,
            [](const ptr<sigproc> &a, sigproc *b)
            {
                return a.get() < b;
            });
    if (it != endsigp && it->get() != sproc)
        it = endsigp;

    return it - begsigp;
}
//------------------------------------------------------------------------------
sigproc *mimosystem::connect(sigproc *sproc, unsigned in, signode *node)
{
    return connect(index(sproc), in, node);
}
//------------------------------------------------------------------------------
sigproc *mimosystem::connect(unsigned idx, unsigned in, signode *node)
{
    // Connect {sigprocs_[idx], in} input to node
    
    auto to_it = connections_.find(node);

    if (to_it == end(connections_) && node)
        return nullptr;

    if (idx >= sigprocs_.size())
        return nullptr;
    
    const auto sproc = sigprocs_[idx].get();

    const auto from_node = sproc->innode(in);
    
    auto from_it = connections_.find(from_node);
    if (from_it != connections_.end())
    {
        from_it->second.erase({sproc, in});
    }

    if (!sproc->connect(in, node))
        return nullptr;

    to_it->second.insert({sproc, in});

    return sproc;
}
//------------------------------------------------------------------------------
sigproc *mimosystem::disconnect(sigproc *sproc, unsigned in)
{
    return disconnect(index(sproc), in);
}
//------------------------------------------------------------------------------
sigproc *mimosystem::disconnect(unsigned idx, unsigned in)
{
    return connect(idx, in, nullptr);
}
//------------------------------------------------------------------------------
signode *mimosystem::link(signode *node, unsigned out)
{
    if (out > outnodes_.size())
        return nullptr;

    auto endoutn = end(outnodes_);
    auto begoutn = begin(outnodes_);

    auto it = find(begoutn, endoutn, node);
    if (it != endoutn)
        return nullptr;

    outnodes_.emplace(begoutn + out, node);

    return node;
}
//------------------------------------------------------------------------------
signode *mimosystem::linknext(signode *node)
{
    return link(node, outnodes_.size());
}
//------------------------------------------------------------------------------
signode *mimosystem::unlink(unsigned out)
{
    if (out >= outnodes_.size())
        return nullptr;

    outnodes_.erase(begin(outnodes_) + out);
}
//------------------------------------------------------------------------------
void mimosystem::newsigproc(ptr<sigproc> pin)
{
    const auto p = pin.get();

    // insert into sigprocs_ the new sigproc
    sigprocs_.emplace(
            upper_bound(begin(sigprocs_), end(sigprocs_), p,
                [](sigproc *a, const ptr<sigproc> &b)
                {
                    return a < b.get();
                }), move(pin)
            );

    // If this can be clocked, add to sigproclkd
    const auto sclk = dynamic_cast<sigproclkd*>(p);
    if (sclk)
    {
        sigproclkds_.emplace(sclk);
    }

    // Add all outnodes to the connection map, initialized with an empty set
    const auto numoutnodes = p->num_outnodes();

    for (unsigned i = 0; i < numoutnodes; ++i)
    {
        auto pout = p->outnode(i);
        pout->commit(current_clock_ - 1, 0);
        connections_[pout] = {};
    }
}
//------------------------------------------------------------------------------
void mimosystem::unlinkall()
{
    outnodes_.clear();
}
//------------------------------------------------------------------------------

} //namespace dspunit

