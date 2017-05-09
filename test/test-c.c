/**
 * @file   test/test-cpp.cpp
 * @date   02.05.2017
 * @author Klemens D. Morgenstern
 *
 * Published under [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0.html)
  <pre>
    /  /|  (  )   |  |  /
   /| / |   \/    | /| /
  / |/  |   /\    |/ |/
 /  /   |  (  \   /  |
               )
 </pre>
 */

#include <mw/test/calltrace.h>
#include <stdio.h>

int errored = 0;

void check(int condition, const char* condition_str, const char* file, int line)
{
    if (!condition)
    {
        errored = 1;
        printf("%s(%i): Test failed: \"%s\"\n", file, line, condition_str);
    }
}
#define CHECK(Condition) check(Condition, #Condition, __FILE__, __LINE__)

#define TEST_CASE(Name) \
void Test_##Name(); \
void Name() \
{ \
   printf("Entering test case %s\n", #Name); \
   Test_##Name(); \
   printf("Exiting test case %s\n", #Name); \
} \
void Test_##Name()


void unwatched() {};

void foo() {}
void bar() {}
void foobar() {foo(); bar();}


TEST_CASE(simple)
{

    const void * ct_arr[] = {&foo, &bar};
    mw_calltrace ct = {&foobar, ct_arr, 2, 0, 0};
    const void * ct_fail_arr[] = {&bar, &foo};
    mw_calltrace ct_fail = {&foobar, ct_fail_arr, 2, 0, 0};

    const void * ct_short_arr[] = {&foo};
    mw_calltrace ct_short = {&foobar, ct_short_arr, 1, 0, 0};

    const void * ct_long_arr[] = {&foo, &bar, &foo};
    mw_calltrace ct_long = {&foobar, ct_long_arr, 3, 0, 0};

    CHECK(mw_calltrace_init(&ct));
    CHECK(mw_calltrace_init(&ct_fail));

    CHECK(mw_calltrace_init(&ct_short));
    CHECK(mw_calltrace_init(&ct_long));


    CHECK(!mw_calltrace_success(&ct));
    CHECK(!ct.errored);
    CHECK(!mw_calltrace_complete(&ct));

    CHECK(!mw_calltrace_success(&ct_fail));
    CHECK(!ct_fail.errored);
    CHECK(!mw_calltrace_complete(&ct_fail));


    foobar();
    unwatched();

    CHECK(mw_calltrace_success(&ct));
    CHECK(mw_calltrace_complete(&ct));
    CHECK(!ct.errored);

    CHECK(!mw_calltrace_success(&ct_fail));
    CHECK(mw_calltrace_complete(&ct_fail));
    CHECK(ct_fail.errored);

    CHECK(!mw_calltrace_success(&ct_short));
    CHECK(mw_calltrace_complete(&ct_short));
    CHECK(ct_short.errored);

    CHECK(!mw_calltrace_success(&ct_long));
    CHECK(mw_calltrace_complete(&ct_long));
    CHECK(ct_long.errored);

    CHECK(mw_calltrace_deinit(&ct_short));
    CHECK(mw_calltrace_deinit(&ct_long));
    CHECK(mw_calltrace_deinit(&ct_fail));
    CHECK(mw_calltrace_deinit(&ct));

}

void foo_switch(int switch_)
{
    foo();
    bar();
    if (switch_)
        foo();
    else
        bar();
}

TEST_CASE(repetition)
{
    const void * ct_arr[]   = {&foo, &bar, &bar};
    mw_calltrace ct = {&foo_switch, ct_arr, 3, 0, 0};

    const void * ct_once_arr[]   = {&foo, &bar, &bar};
    mw_calltrace ct_once = {&foo_switch, ct_once_arr, 3, 1, 0};

    const void * ct_skip_arr[]   = {&foo, &bar, &foo};
    mw_calltrace ct_skip = {&foo_switch, ct_skip_arr, 3, 0, 1};

    const void * ct_rep_arr[]   = {&foo, &bar, &bar};
    mw_calltrace ct_rep = {&foo_switch, ct_rep_arr, 3, 2, 0};

    const void * ct_flex_arr[]   = {&foo, &bar, 0};
    mw_calltrace ct_flex = {&foo_switch, ct_flex_arr, 3, 2, 0};

    CHECK(mw_calltrace_init(&ct));
    CHECK(mw_calltrace_init(&ct_once));
    CHECK(mw_calltrace_init(&ct_skip));
    CHECK(mw_calltrace_init(&ct_rep));
    CHECK(mw_calltrace_init(&ct_flex));

    unwatched();
    foo_switch(0);

    CHECK(mw_calltrace_success(&ct));
    unwatched();
    foo_switch(1);
    unwatched();

    CHECK(mw_calltrace_complete(&ct));
    CHECK(mw_calltrace_complete(&ct_once));
    CHECK(mw_calltrace_complete(&ct_skip));
    CHECK(mw_calltrace_complete(&ct_rep));
    CHECK(mw_calltrace_complete(&ct_flex));

    CHECK(!mw_calltrace_success(&ct));
    CHECK(mw_calltrace_success(&ct_once));
    CHECK(mw_calltrace_success(&ct_skip));
    CHECK(!mw_calltrace_success(&ct_rep));
    CHECK(mw_calltrace_success(&ct_flex));

    CHECK(mw_calltrace_deinit(&ct));
    CHECK(mw_calltrace_deinit(&ct_once));
    CHECK(mw_calltrace_deinit(&ct_skip));
    CHECK(mw_calltrace_deinit(&ct_rep));
    CHECK(mw_calltrace_deinit(&ct_flex));

}

void func()
{
    foo_switch(1);
    foobar();
    foo_switch(0);
}

TEST_CASE(nested)
{
    const void * ct_func_arr[]   = {&foo_switch, &foobar, &foo_switch};
    mw_calltrace ct_func = {&func, ct_func_arr, 3, 0, 0};

    const void * ct_switch_arr[]   = {&foo, &bar, 0};
    mw_calltrace ct_switch = {&foo_switch, ct_switch_arr, 3,  2, 0};

    const void * ct_fb_arr[] = {&foo, &bar};
    mw_calltrace ct_fb = {&foobar, ct_fb_arr, 2, 0, 0};

    const void * ct_1_arr[]   = {&foo, &bar, &foo};
    mw_calltrace ct_1 = {&foo_switch, ct_1_arr, 3, 1, 0};

    const void * ct_2_arr[]   = {&foo, &bar, &bar};
    mw_calltrace ct_2 = {&foo_switch, ct_2_arr, 3, 1, 1};

    CHECK(mw_calltrace_init(&ct_func));
    CHECK(mw_calltrace_init(&ct_switch));
    CHECK(mw_calltrace_init(&ct_fb));
    CHECK(mw_calltrace_init(&ct_1));
    CHECK(mw_calltrace_init(&ct_2));

    unwatched();
    func();
    unwatched();

    CHECK(mw_calltrace_success(&ct_func));
    CHECK(mw_calltrace_success(&ct_switch));
    CHECK(mw_calltrace_success(&ct_fb));
    CHECK(mw_calltrace_success(&ct_1));
    CHECK(mw_calltrace_success(&ct_2));

    CHECK(mw_calltrace_deinit(&ct_func));
    CHECK(mw_calltrace_deinit(&ct_switch));
    CHECK(mw_calltrace_deinit(&ct_fb));
    CHECK(mw_calltrace_deinit(&ct_1));
    CHECK(mw_calltrace_deinit(&ct_2));}

TEST_CASE(ovl)
{
    const void * arr[] = {&bar};
    mw_calltrace ct_0 = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_1 = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_2 = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_3 = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_4 = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_5 = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_6 = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_7 = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_8 = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_9 = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_A = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_B = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_C = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_D = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_E = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_F = {&foo, arr, 1, 0, 0};
    mw_calltrace ct_10= {&foo, arr, 1, 0, 0};


    CHECK(mw_calltrace_init(&ct_0));
    CHECK(mw_calltrace_init(&ct_1));
    CHECK(mw_calltrace_init(&ct_2));
    CHECK(mw_calltrace_init(&ct_3));
    CHECK(mw_calltrace_init(&ct_4));
    CHECK(mw_calltrace_init(&ct_5));
    CHECK(mw_calltrace_init(&ct_6));
    CHECK(mw_calltrace_init(&ct_7));
    CHECK(mw_calltrace_init(&ct_8));
    CHECK(mw_calltrace_init(&ct_9));
    CHECK(mw_calltrace_init(&ct_A));
    CHECK(mw_calltrace_init(&ct_B));
    CHECK(mw_calltrace_init(&ct_C));
    CHECK(mw_calltrace_init(&ct_D));
    CHECK(mw_calltrace_init(&ct_E));
    CHECK(mw_calltrace_init(&ct_F));

    CHECK(!mw_calltrace_init(&ct_10));
    CHECK(!mw_calltrace_deinit(&ct_10));

    CHECK(mw_calltrace_deinit(&ct_0));
    CHECK(mw_calltrace_deinit(&ct_1));
    CHECK(mw_calltrace_deinit(&ct_2));
    CHECK(mw_calltrace_deinit(&ct_3));
    CHECK(mw_calltrace_deinit(&ct_4));
    CHECK(mw_calltrace_deinit(&ct_5));
    CHECK(mw_calltrace_deinit(&ct_6));
    CHECK(mw_calltrace_deinit(&ct_7));
    CHECK(mw_calltrace_deinit(&ct_8));
    CHECK(mw_calltrace_deinit(&ct_9));
    CHECK(mw_calltrace_deinit(&ct_A));
    CHECK(mw_calltrace_deinit(&ct_B));
    CHECK(mw_calltrace_deinit(&ct_C));
    CHECK(mw_calltrace_deinit(&ct_D));
    CHECK(mw_calltrace_deinit(&ct_E));
    CHECK(mw_calltrace_deinit(&ct_F));
}

void recurse(int cnt)
{
    if (cnt > 0)
        recurse(cnt - 1);
}

TEST_CASE(recursion)
{
    const void * arr[] = {&recurse};
    mw_calltrace ct0 = {&recurse, arr, 1, 1, 0};
    mw_calltrace ct1 = {&recurse, arr, 1, 1, 1};
    mw_calltrace ct2 = {&recurse, arr, 1, 1, 2};
    mw_calltrace ct3 = {&recurse, arr, 1, 1, 3};

    CHECK(mw_calltrace_init(&ct0));
    CHECK(mw_calltrace_init(&ct1));
    CHECK(mw_calltrace_init(&ct2));
    CHECK(mw_calltrace_init(&ct3));

    unwatched();
    recurse(4);
    unwatched();

    CHECK(mw_calltrace_success(&ct0));
    CHECK(mw_calltrace_success(&ct1));
    CHECK(mw_calltrace_success(&ct2));
    CHECK(mw_calltrace_success(&ct3));

    CHECK(mw_calltrace_deinit(&ct0));
    CHECK(mw_calltrace_deinit(&ct1));
    CHECK(mw_calltrace_deinit(&ct2));
    CHECK(mw_calltrace_deinit(&ct3));

}

int main(int argc, const char* argv[])
{
    simple();
    repetition();
    nested();
    ovl();
    recursion();
    return errored;
}
