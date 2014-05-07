#include "signode.hpp"
//------------------------------------------------------------------------------
#include "sigproc.hpp"
#include <sstream>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

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
float signode::out(unsigned inclock)
{
    if (clockref_ != inclock)
    {
        clockref_ = inclock;
        sigproc_->update(inclock, value_);
    }
    return value_;
}
//------------------------------------------------------------------------------
void signode::update(unsigned inclock, float val)
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
bool signode::value(unsigned inclock, float &v) const
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
    return &::nullnode;
}
//------------------------------------------------------------------------------

