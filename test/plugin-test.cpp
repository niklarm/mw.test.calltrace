/**
 * @file   test/plugin-test.cpp
 * @date   10.05.2017
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

#include <mw/test/calltrace.hpp>

#include <chrono>
#include <iostream>

#if defined(MW_TEST_TIMESTAMP)

mw::test::mw_timestamp_t mw::test::timestamp()
{
    return std::chrono::system_clock::now().time_since_epoch().count();
}

#endif

template<typename T>
struct dummy_vector
{
    dummy_vector() {};

    void push_back(T &&) const {};
    void size() const {};
};

dummy_vector<int> dummy;

void bar() {dummy.size();}
void foo() {dummy.push_back(42);}
void foobar() {foo(); bar();}


int main(int argc, char * argv[])
{
    using namespace mw::test;

    calltrace<2> ct      (&foobar, &foo, &bar);
    calltrace<2> ct_fail (&foobar, &bar, &foo);

    calltrace<1> ct_short(&foobar, &foo);
    calltrace<3> ct_long (&foobar, &foo, &bar, &foo);

    calltrace<1> ct_size     (&bar, &dummy_vector<int>::size);
    calltrace<1> ct_push_back(&foo, &dummy_vector<int>::push_back);

    foobar();

    int ret = 0;
    if (!ct)           ret += 0b000001;
    if ( ct_fail)      ret += 0b000010;
    if ( ct_short)     ret += 0b000100;
    if ( ct_long)      ret += 0b001000;
    if (!ct_size)      ret += 0b010000;
    if (!ct_push_back) ret += 0b100000;

    return ret;
}

