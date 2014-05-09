#include "signode.hpp"
//------------------------------------------------------------------------------
#include "sigproc.hpp"
#include <sstream>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

namespace dspunit
{

signode nullnode {sigproc::nullproc()};

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
signode::signode()
    : signode(sigproc::nullproc())
{
}
//------------------------------------------------------------------------------
signode::signode(sigproc *sproc) 
    : sigproc_{sproc}
{
    name = "Node ";
    std::stringstream ss;
    ss << this;
    name += std::move(ss).str();
}
//------------------------------------------------------------------------------
sigvalue_t signode::update(clockref_t inclock)
{
    if (clockref_ != inclock)
    {
        clockref_ = inclock;
        sigproc_->update(inclock, value_);
    }
    return value_;
}
//------------------------------------------------------------------------------
void signode::commit(clockref_t inclock, sigvalue_t val)
{
    clockref_ = inclock;
    value_    = val;
}
//------------------------------------------------------------------------------
void signode::connect(sigproc *sproc)
{
    sigproc_ = sproc;
}
//------------------------------------------------------------------------------
void signode::disconnect()
{
    connect(sigproc::nullproc());
}
//------------------------------------------------------------------------------
bool signode::value(clockref_t inclock, sigvalue_t &v) const
{
    const bool b = clockref_ == inclock;
    if (b)
    {
        v = value_;
    }
    return b;
}
//------------------------------------------------------------------------------
signode *signode::nullnode()
{
    return &::dspunit::nullnode;
}
//------------------------------------------------------------------------------

} // namespace dspunit

