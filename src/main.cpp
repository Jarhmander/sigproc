#include <iostream>
//------------------------------------------------------------------------------
#include "mimosystem.hpp"
#include "sigblocks.hpp"
//------------------------------------------------------------------------------

using namespace std;

namespace dsp = dspunit;

int main()
{
    dsp::mimosystem mms {};

    auto c_adder00 = mms.create<dsp::adder>();
    auto c_const00 = mms.create<dsp::constant>(2);
    auto c_const01 = mms.createfrom([]() { return cout << "Lolzor" << endl, new dsp::constant {3}; });
    auto c_sgain00 = mms.create<dsp::gain>(1.565);
    auto c_sgain01 = mms.create<dsp::gain>(-.64);
    auto c_delay00 = mms.create<dsp::delay1>();
    auto c_delay01 = mms.create<dsp::delay1>();

    c_adder00->resize_innodes(4);

    cout << mms.connect(c_adder00, 0, c_const00->outnode(0));
    cout << endl;
    cout << mms.connect(c_adder00, 1, c_const01->outnode(0));
    cout << endl;
    cout << mms.connect(c_adder00, 2, c_delay00->outnode(0));
    cout << endl;
    cout << mms.connect(c_adder00, 3, c_delay01->outnode(0));
    cout << endl;

    cout << mms.connect(c_sgain00, 0, c_adder00->outnode(0));
    cout << endl;

    cout << mms.connect(c_delay00, 0, c_sgain00->outnode(0));
    cout << endl;

    cout << mms.connect(c_sgain01, 0, c_delay00->outnode(0));
    cout << endl;

    cout << mms.connect(c_delay01, 0, c_sgain01->outnode(0));
    cout << endl;

    cout << mms.linknext(c_adder00->outnode(0));
    cout << endl;

    for (int i = 0; i<8; ++i)
       cout << "System output is: " << mms.update()[0]->value() << endl;

    mms.destroy(c_const01);


    for (int i = 0; i<8; ++i)
        cout << "System output is: " << mms.update()[0]->value() << endl;
}

