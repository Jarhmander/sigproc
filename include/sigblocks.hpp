#ifndef SIGBLOCKS_HPP
#define SIGBLOCKS_HPP
//------------------------------------------------------------------------------
#include "sigproc.hpp"
#include "signode.hpp"
#include <vector>
//------------------------------------------------------------------------------

template <typename T>
 using vec = std::vector<T>;

using vecnodes = vec<signode *>;

class constant : public tsigproc<1,sigproc::fixed,0>
{
public:
    float value = 0.f;

    constant(float v) : value{v} {}

    void set(float v) { value = v; }
    float get() const { return value; }

    void update(unsigned, float &v) override
    {
        v = value;
    }
    unsigned num_innodes() const override { return 0; }
    unsigned num_outnodes() const override { return 1; }
};

class adder : public tsigproc<1,sigproc::fixed,2>
{
public:
    void update(unsigned inclock, float &v) override
    {
        float out = 0.f;
        for (auto &in : inputs_)
        {
            out += in->out(inclock);
        }
        v = out;
    }
};

//------------------------------------------------------------------------------
#endif // SIGBLOCKS_HPP

