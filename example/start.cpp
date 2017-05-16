#include <mw/test/calltrace.hpp>
#include <cassert>

void foo2();
void bar2();

//[start_example

void foo()
{
    foo2();
}

void bar()
{
    bar2();
}

void func()
{
    foo();
    bar();
}

void test_func()
{
    mw::test::calltrace<2> ct
        {
         &func,
         &foo, &bar
        };

    func();

    assert(ct);
}
//]

//[start_example_ext
void test_func_ext()
{
    mw::test::calltrace<2> ct
        {
         &func,
         &foo, &bar
        };

    mw::test::calltrace<1> ct_foo
        {
         &foo,
         &foo2
        };

    mw::test::calltrace<1> ct_bar
        {
         &bar,
         &bar2
        };

    func();

    assert(ct);
    assert(ct_foo);
    assert(ct_bar);
}
//]
