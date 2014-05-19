#include "mimosystem.hpp"
//------------------------------------------------------------------------------
#include "sigproc.hpp"
#include "signode.hpp"
#include "vecset.hpp"
#include <algorithm>
#include <iterator>
//------------------------------------------------------------------------------

namespace dspunit
{

using std::begin;
using std::end;
using std::find;
using std::binary_search;
using std::move;
using std::copy;
using std::make_move_iterator;
using std::lower_bound;

struct nodelete
{
    void operator()(...) const {}
};

template <typename T>
 using fake_ptr = std::unique_ptr<T, nodelete>;

template <typename T>
 fake_ptr<T> make_fake(T *t)
{
    return fake_ptr<T> {t};
}

//------------------------------------------------------------------------------
mimosystem::mimosystem()
{
    sigprocs_.reserve(32);
    connections_.reserve(32);
    sigproclkds_.reserve(32);
}
//------------------------------------------------------------------------------
auto mimosystem::update() -> const vec<signode *> &
{
    updateout();
    ++current_clock_;
    updateout();
    clock();

    return outnodes_;
}
//------------------------------------------------------------------------------
void mimosystem::destroy(sigproc *sproc)
{
    auto it = ::dspunit::find(sigprocs_, make_fake(sproc) );
    if (it == end(sigprocs_))
        return;

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
sigproc *mimosystem::connect(sigproc *sproc, unsigned in, signode *node)
{
    // Connect {sigprocs_[idx], in} input to node

    auto to_it = connections_.find(node);

    if (to_it == end(connections_) && node)
        return nullptr;

    if (!binary_search(begin(sigprocs_), end(sigprocs_), make_fake(sproc)))
        return nullptr;

    const auto from_node = sproc->innode(in);

    auto from_it = connections_.find(from_node);
    if (from_it != connections_.end())
    {
        from_it->second.erase({sproc, in});
    }

    if (!sproc->connect(in, node))
        return nullptr;

    if (node)
    {
        to_it->second.insert({sproc, in});
    }

    return sproc;
}
//------------------------------------------------------------------------------
sigproc *mimosystem::disconnect(sigproc *sproc, unsigned in)
{
    return connect(sproc, in, nullptr);
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
mimosystem &mimosystem::operator+=(mimosystem &&a)
{
    sigprocs_.reserve(sigprocs_.size() + a.sigprocs_.size());
    outnodes_.reserve(outnodes_.size() + a.outnodes_.size());

    // move things into outnodes
    outnodes_.insert(outnodes_.end(),  a.outnodes_.begin(), a.outnodes_.end() );

    // move things into sigprocs_
    insert_sort_range(sigprocs_,
            make_move_iterator(a.sigprocs_.begin()),
            make_move_iterator(a.sigprocs_.end()));

    // move things into connections_
    connections_.insert(
            make_move_iterator(a.connections_.begin()),
            make_move_iterator(a.connections_.end())
            );

    // move things into sigproclkds_
    sigproclkds_.insert(
            (a.sigproclkds_.begin()),
            (a.sigproclkds_.end())
            );
    return *this;
}
//------------------------------------------------------------------------------
void mimosystem::unlinkall()
{
    outnodes_.clear();
}
//------------------------------------------------------------------------------
void mimosystem::newsigproc(ptr<sigproc> pin)
{
    const auto p = pin.get();

    // insert into sigprocs_ the new sigproc
    insert_sort(sigprocs_, move(pin));

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
void mimosystem::updateout()
{
    for (auto &e : outnodes_)
    {
        e->update(current_clock_);
    }
}
//------------------------------------------------------------------------------
void mimosystem::clock()
{
    ++current_clock_;
    for (auto &sclk : sigproclkds_)
    {
        sclk->clock();
    }
}
//------------------------------------------------------------------------------


} //namespace dspunit

