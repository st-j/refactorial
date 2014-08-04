#include <vector>

#include "ClassRename.h"
#include "FunctionRename.h"

int main(int, char**)
{
    C::ElNada nada;
    nada.f->doFooter();

    A::Foo mySecondClass;
    mySecondClass.getZeroFoo();

    std::vector<int> test;
    test.push_back(1);

    SampleNameSpace::Foo dut;
    dut.doNothing();
    dut.setX(dut.getX());
}
