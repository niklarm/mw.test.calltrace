#include <mw/test/calltrace.hpp>
#include <cassert>


//[wildcard
struct foo
{
    foo();
    void bar();
    ~foo();
};

void func()
{
    foo f;
    f.bar();
}

void test_func()
{
    mw::test::calltrace<3> ct
        {
         &func,
         mw::test::any_fn, /*<The constructor call>*/
         &foo::bar,
         mw::test::any_fn  /*<The destructor call>*/
        };

    func();

    assert(ct);
}
//]

