#include <mw/test/calltrace.hpp>
#include <cassert>


//[repeat_example
void foo();
void bar();
void bar2();

void func(bool switch_)
{
    foo();
    if (switch_)
        bar();
    else
        bar2();
}

void test_func()
{
    mw::test::calltrace<2> ct
        {
         &func,
         &foo,
         mw::test::any_fn /*<Wildcard for `bar` or `bar2`>*/
        };

    mw::test::calltrace<2> ct_start
        {
         &func,
         2, /*<Repeat two times>*/
         &foo, &bar
        };

    mw::test::calltrace<2> ct_middle
        {
         &func,
         2, /*<Repeat two times>*/
         2, /*<Offset, i.e. ignore the first two>*/
         &foo, &bar2
        };

    mw::test::calltrace<2> ct_end
        {
         &func,
         1, /*<Repeat only once>*/
         4, /*<Offset,i.e. ignore the first four>*/
         &foo, &bar
        };

    func(true);
    func(true);
    func(false);
    func(false);
    func(true);

    assert(ct);
    assert(ct_start);
    assert(ct_middle);
    assert(ct_end);
}
//]

