#include <mw/test/calltrace.h>
#include <assert.h>

void foo2();
void bar2();

//[repeat_example_c
void foo();
void bar();
void bar2();

void func(int switch_)
{
    foo();
    if (switch_ > 0)
        bar();
    else
        bar2();
}

void test_func()
{
    const void * ct_arr[] = {
         &foo,
         0 };/*<Wildcard for bar or bar2>*/
    mw_calltrace ct =
        {
         &func,
         ct_arr, 2,
         0, 0 /*<Repeat as often as called and skip nothing>*/
        };
    assert(mw_calltrace_init(&ct));

    const void* ct_start_arr[] = {&foo, &bar};
    mw_calltrace ct_start =
        {
         &func,
         ct_start_arr, 2,
         2, /*<Repeat two times>*/
         0  /*<No skip>*/
        };
    assert(mw_calltrace_init(&ct_start));

    const void* ct_middle_arr[] = {&foo, &bar2};
    mw_calltrace ct_middle =
        {
         &func,
         ct_middle_arr, 2,
         2, /*<Repeat two times>*/
         2 /*<Offset, i.e. ignore the first two>*/
        };
    assert(mw_calltrace_init(&ct_middle));

    mw_calltrace ct_end =
        {
         &func,
         ct_start_arr, 2,
         1, /*<Repeat only once>*/
         4  /*<Offset,i.e. ignore the first four>*/
        };

    func(1);
    func(1);
    func(0);
    func(0);
    func(1);

    assert(mw_calltrace_success(&ct));
    assert(mw_calltrace_success(&ct_start));
    assert(mw_calltrace_success(&ct_middle));
    assert(mw_calltrace_success(&ct_end));

    assert(mw_calltrace_deinit(&ct));
    assert(mw_calltrace_deinit(&ct_start));
    assert(mw_calltrace_deinit(&ct_middle));
    assert(mw_calltrace_deinit(&ct_end));
}
//]

