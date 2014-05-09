#ifndef SIGNODE_HPP
#define SIGNODE_HPP
//------------------------------------------------------------------------------
#include <string>
//------------------------------------------------------------------------------

namespace dspunit
{

class sigproc;

class signode
{
public:
    signode();
    signode(sigproc *sproc);
    float update(unsigned inclock);
    void  commit(unsigned inclock, float value);

    void connect(sigproc *);
    void disconnect();

    sigproc *proc() const { return sigproc_; }
    bool value(unsigned inclock, float &v) const;
    float value() const { return value_; }

    std::string name;

    static signode *nullnode();
private:
    unsigned clockref_ = -1u;
    float value_       = 0.f;
    sigproc *sigproc_;
};

} // namespace dspunit

//------------------------------------------------------------------------------
#endif // SIGNODE_HPP
