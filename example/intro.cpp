//[intro
#include <mw/test/calltrace.hpp>

void foo();
void bar();
void func() {foo(); bar();}

int main(int argc, char *argv[])
{
    //Check that func calls foo and bar in this order.
    mw::test::calltrace<2> ct{&func,
                           &foo, & bar};

    func();

    return ct ? 0 : 1;
}
//]
