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

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <mw/test/calltrace.hpp>

using namespace mw::test;

void foo() {}
void bar() {}
void foobar() {foo(); bar();}


BOOST_AUTO_TEST_CASE(simple)
{

    calltrace<2> ct      (&foobar, &foo, &bar);
    calltrace<2> ct_fail (&foobar, &bar, &foo);

    calltrace<1> ct_short(&foobar, &foo);
    calltrace<3> ct_long (&foobar, &foo, &bar, &foo);

    BOOST_CHECK(!ct);
    BOOST_CHECK(!ct.errored());
    BOOST_CHECK(ct.inited());
    BOOST_CHECK(!ct.complete());

    BOOST_CHECK(!ct_fail);
    BOOST_CHECK(!ct_fail.errored());
    BOOST_CHECK(ct_fail.inited());
    BOOST_CHECK(!ct_fail.complete());

    foobar();

    BOOST_CHECK(ct);
    BOOST_CHECK(!ct.errored());
    BOOST_CHECK(ct.complete());

    BOOST_CHECK(!ct_fail);
    BOOST_CHECK(ct_fail.errored());
    BOOST_CHECK(ct_fail.complete());

    BOOST_CHECK(!ct_short);
    BOOST_CHECK(ct_short.errored());
    BOOST_CHECK(ct_short.complete());

    BOOST_CHECK(!ct_long);
    BOOST_CHECK(ct_long.errored());
    BOOST_CHECK(ct_long.complete());
}

void foo_switch(bool switch_)
{
    foo();
    bar();
    if (switch_)
        foo();
    else
        bar();
}

BOOST_AUTO_TEST_CASE(repetition)
{
    calltrace<3> ct     (&foo_switch, &foo, &bar, &bar);
    calltrace<3> ct_once(&foo_switch, 1, &foo, &bar, &bar);
    calltrace<3> ct_skip(&foo_switch, 0, 1, &foo, &bar, &foo);
    calltrace<3> ct_rep (&foo_switch, 2, &foo, &bar, &bar);
    calltrace<3> ct_flex(&foo_switch, 2, &foo, &bar, any_fn);

    BOOST_CHECK(ct.inited());
    BOOST_CHECK(ct_once.inited());
    BOOST_CHECK(ct_skip.inited());
    BOOST_CHECK(ct_rep.inited());
    BOOST_CHECK(ct_flex.inited());

    foo_switch(false);

    BOOST_CHECK(ct);

    foo_switch(true);


    BOOST_CHECK(ct.complete());
    BOOST_CHECK(ct_once.complete());
    BOOST_CHECK(ct_skip.complete());
    BOOST_CHECK(ct_rep.complete());
    BOOST_CHECK(ct_flex.complete());

    BOOST_CHECK(!ct);
    BOOST_CHECK(ct_once);
    BOOST_CHECK(ct_skip);
    BOOST_CHECK(!ct_rep);
    BOOST_CHECK(ct_flex);
}
