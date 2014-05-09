#ifndef SIGBLOCKS_HPP
#define SIGBLOCKS_HPP
//------------------------------------------------------------------------------
#include "sigproc.hpp"
#include "signode.hpp"
#include <vector>
//------------------------------------------------------------------------------

namespace dspunit
{

class constant : public tsigproc_infixed<1,0>
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
};

class adder : public tsigproc_invar<1,2>
{
public:
    void update(unsigned inclock, float &v) override
    {
        float out = 0.f;
        for (auto &in : inputs_)
        {
            out += in->update(inclock);
        }
        v = out;
    }
};

class gain   : public tsigproc_infixed<1,1>
{
public:
    gain(float a) { value = a; }

    float value = 1.f;

    void update(unsigned inclock, float &v) override
    {
        v = inputs_[0]->update(inclock) * value;
    }
};

class delay1 : public tsigproc_infixed<1,1>, public sigproclkd
{
    float z = 0.f;
public:
    void update(unsigned inclock, float &v) override
    {
        v = z;
        inputs_[0]->update(inclock);
    }
    void clock() override
    {
        z = inputs_[0]->value();
    }
};

} // namespace dspunit

//------------------------------------------------------------------------------
#endif // SIGBLOCKS_HPP

