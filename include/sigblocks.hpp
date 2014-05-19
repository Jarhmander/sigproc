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
    sigvalue_t value = {};

    constant(sigvalue_t v) : value{v} {}

    void set(sigvalue_t v) { value = v; }
    sigvalue_t get() const { return value; }

    void update(clockref_t inclock) override
    {
        outputs_[0].commit(inclock, value);
    }
};

class adder : public tsigproc_invar<1,2>
{
public:
    void update(clockref_t inclock) override
    {
        sigvalue_t out = 0;
        for (auto &in : inputs_)
        {
            out += in->update(inclock);
        }
        outputs_[0].commit(inclock, out);
    }
};

class gain   : public tsigproc_infixed<1,1>
{
public:
    gain(sigvalue_t a) { value = a; }

    sigvalue_t value = 1;

    void update(clockref_t inclock) override
    {
        const auto in = inputs_[0]->update(inclock) * value;
        outputs_[0].commit(inclock, in);
    }
};

class delay1 : public tsigproc_infixed<1,1>, public sigproclkd
{
    sigvalue_t z = {};
public:
    void update(clockref_t inclock) override
    {
        outputs_[0].commit(inclock, z);
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

