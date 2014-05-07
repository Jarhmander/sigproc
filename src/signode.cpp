#include "signode.hpp"
//------------------------------------------------------------------------------
#include "sigproc.hpp"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static struct nullproc_t : tsigproc<1,sigproc::fixed,0>
{
    void update(unsigned, float &v) override { v = 0.f; }
} nullproc;

signode nullnode {&nullproc};

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
signode::signode()
    : signode(&::nullproc)
{
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
    connect(&::nullproc);
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
sigproc *signode::nullproc()
{
    return &::nullproc;
}
//------------------------------------------------------------------------------
signode *signode::nullnode()
{
    return &::nullnode;
}
//------------------------------------------------------------------------------

