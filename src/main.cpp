#include <iostream>
//------------------------------------------------------------------------------
#include "mimosystem.hpp"
#include "sigblocks.hpp"
//------------------------------------------------------------------------------

using namespace std;

int main()
{
    cout << "Hello moon." << endl;
    mimosystem mms {};

    auto c_adder00 = mms.create<adder>();
    auto c_const00 = mms.create<constant>(2);
    auto c_const01 = mms.create<constant>(3);

    cout << c_adder00->connect(0, c_const00->outnode(0));
    cout << endl;
    cout << c_adder00->connect(1, c_const01->outnode(0));
    cout << endl;

    cout << "System output is: " << c_adder00->outnode(0)->out(3) << endl;

}

