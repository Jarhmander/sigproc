#ifndef SIGNODE_HPP
#define SIGNODE_HPP
//------------------------------------------------------------------------------

class sigproc;

class signode
{
public:
    signode();
    signode(sigproc *sproc) : sigproc_{sproc} {}
    float out(unsigned inclock);
    void  update(unsigned inclock, float value);

    void connect(sigproc *);
    void disconnect();

    sigproc *proc() const { return sigproc_; }
    bool value(unsigned inclock, float &v) const;
    float value() const { return value_; }

    static sigproc *nullproc();
    static signode *nullnode();
private:
    unsigned clockref_ = -1u;
    float value_       = 0.f;
    sigproc *sigproc_;
};

//------------------------------------------------------------------------------
#endif // SIGNODE_HPP
