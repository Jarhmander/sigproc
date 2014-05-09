#ifndef SIGPROC_HPP
#define SIGPROC_HPP
//------------------------------------------------------------------------------
#include "signode.hpp"
#include <array>
#include <vector>
//------------------------------------------------------------------------------

namespace dspunit
{

class sigproc
{
    friend class mimosystem;

public:
    // dtor
    virtual ~sigproc() {}

    // signode::update calls sigproc::update when necessary.
    // sigprocs can update directly the float reference if there's only one
    // output, if desired. Otherwise, it must update all its output nodes by
    // calling commit on them with the final values of the respective nodes.
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

    // Give the Nth input node, 0-based. If input doesn't exist, returns
    // nullptr.
    virtual signode *innode(unsigned) const = 0;

    // Connect the Nth input to a node, 0-based. If input doesn't exist, returns
    // nullptr, otherwise returns the argument.
protected:
    virtual signode *connect(unsigned, signode *) = 0;

    signode *disconnect(unsigned idx)
    {
        return connect(idx, signode::nullnode());
    }
public:

    static sigproc *nullproc();
};

class sigproclkd
{
public:
    virtual ~sigproclkd() {}

    // Sequential circuits (ex: filters) need to be clocked in order to produce
    // new values afterwards.
    virtual void clock() = 0;
};

class sigproc_fixedinputs : public sigproc
{};

class sigproc_varinputs : public sigproc
{
public:
    virtual void resize_innodes(unsigned) = 0;
};

template <typename Base, typename Container>
 class sigproc_inputspecbase : public Base
{
protected:
    signode *connect(unsigned idx, signode *snode) override
    {
        if (idx >= inputs_.size())
            return nullptr;

        if (!snode)
            snode = signode::nullnode();

        inputs_[idx] = snode;

        return snode;
    }
public:
    signode *innode(unsigned idx) const override
    {
        return (idx >= inputs_.size()) ? nullptr : inputs_[idx];
    }

    unsigned num_innodes() const override
    {
        return inputs_.size();
    }

protected:
    Container inputs_;
};

template <typename Container, unsigned num_in>
 class sigproc_varninputs
        : public sigproc_inputspecbase<sigproc_varinputs,Container>
{
public:
    sigproc_varninputs()
    {
        this->inputs_.reserve(num_in);
        this->inputs_.insert(this->inputs_.begin(), num_in, 
                                                        signode::nullnode());
    }
    
    void resize_innodes(unsigned newsz) override
    {
        this->inputs_.resize(newsz,signode::nullnode());
    }
};

template <unsigned num_in>
 class sigproc_fixninputs
        : public sigproc_inputspecbase<
            sigproc_fixedinputs,std::array<signode*,num_in>
          >
{
public:
    sigproc_fixninputs()
    {
        this->inputs_.fill(signode::nullnode());
    }
};

template <unsigned num_out, typename Base>
 class tsigproc : public Base
{
public:
    tsigproc() 
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

protected:
    std::array<signode, num_out> outputs_; 
};

template <unsigned num_out, unsigned num_in>
 using tsigproc_infixed = tsigproc<num_out,sigproc_fixninputs<num_in>>;

template <unsigned num_out, unsigned num_in>
 using tsigproc_invar = tsigproc<num_out,
                            sigproc_varninputs<std::vector<signode*>, num_in>
                            >;

} // namespace dspunit

//------------------------------------------------------------------------------
#endif // SIGPROC_HPP

