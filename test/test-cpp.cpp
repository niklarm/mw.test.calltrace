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

#include <iostream>

void foo() {}
void bar() {}
void foobar() {foo(); bar();}


BOOST_AUTO_TEST_CASE(plain)
{
    using namespace mw::test;
    auto ct = make_calltrace(&foobar, &foo, &bar);

    BOOST_CHECK(!ct);
    BOOST_CHECK(!ct.errored());
    BOOST_CHECK(ct.inited());
    BOOST_CHECK(!ct.complete());

    foobar();

    BOOST_CHECK(ct);
    BOOST_CHECK(!ct.errored());
    BOOST_CHECK(ct.complete());
}

