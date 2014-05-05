#ifndef MIMOSYSTEM_HPP
#define MIMOSYSTEM_HPP
//------------------------------------------------------------------------------
#include <vector>
#include <memory>
//------------------------------------------------------------------------------

class sigproc;

class signode;

class mimosystem
{
public:
    using uint = unsigned int;

private:
    std::vector<std::unique_ptr<sigproc>> sigprocs;
    std::vector<std::unique_ptr<nodes>>   signodes;

public:
    mimosystem() = default;

    float out();

    void  clock();
};

//------------------------------------------------------------------------------
#endif // MIMOSYSTEM_HPP
