#ifndef SIGPROC_HPP
#define SIGPROC_HPP
//------------------------------------------------------------------------------
#include "signode.hpp"
#include <array>
//------------------------------------------------------------------------------


class sigproc
{
public:
    enum nvars { fixed, variadic };

    // Sequential circuits (ex: filters) need to be clocked in order to produce
    // new values afterwards.
    virtual void clock() {}

    // signode calls sigproc::update when necessary.
    // sigprocs can update directly the float reference if there's only one
    // output, if desired. Otherwise, it must update all its output nodes by
    // calling update on them with the final values of the respective nodes.
    virtual void update(unsigned, float &) = 0;

    // Number of input nodes. Note that this isn't necessarily a constant value,
    // that is, it may change if the "component" can change its number of
    // inputs. Can be 0 if it's a source.
    virtual unsigned num_innodes() const = 0;

    // Number of output nodes. Can't be zero, and can't change over time.
    virtual unsigned num_outnodes() const = 0;

    // Give the Nth output node, 0-based. If node doesn't exist, gives a 
    // nullptr.
    virtual signode *outnode(unsigned) = 0;

    // Connect the Nth input to a node, 0-based. If input doesn't exist, returns
    // nullptr, otherwise returns the argument.
    virtual signode *connect(unsigned, signode *) = 0;

    signode *disconnect(unsigned idx)
    {
        return connect(idx, signode::nullnode());
    }
};


template <unsigned num_out>
 class sigproc_outspec : public sigproc
{
public:
    sigproc_outspec() 
    {
        for (auto &i : outputs_)
        {
            i.connect(this);
        }
    }

    unsigned num_outnodes() const override
    {
        return num_out;
    }

    signode *outnode(unsigned idx) override
    {
        return idx >= outputs_.size() ? nullptr : &outputs_[idx];
    }

    enum { n_outputs = num_out };

protected:
    std::array<signode, num_out> outputs_; 
};

template <unsigned num_out, sigproc::nvars, unsigned num_in>
 class tsigproc;

template <unsigned num_out, unsigned num_in>
 class tsigproc<num_out, sigproc::fixed, num_in> : public sigproc_outspec<num_out>
{
public:
    tsigproc()
    {
       inputs_.fill(signode::nullnode());
    } 

    signode *connect(unsigned idx, signode *snode)
    {
        if (idx >= inputs_.size())
            return nullptr;

        if (!snode)
            snode = signode::nullnode();

        inputs_[idx] = snode;
    }

    unsigned num_innodes() const override
    {
        return num_in;
    }

    enum { n_inputs = num_in };
protected:
    std::array<signode *, num_in> inputs_;
};

//------------------------------------------------------------------------------
#endif // SIGPROC_HPP

