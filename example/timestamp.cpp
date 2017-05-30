//[timestamp_example
#include <mw/test/calltrace.hpp>
#include <chrono>

extern "C" { extern int __mw_calltrace_size; }

mw::test::timestamp_t mw::test::timestamp()
{
    int sz = __mw_calltrace_size;
    __mw_calltrace_size = 0;

    auto ts = std::chrono::system_clock::now()
                .time_since_epoch().count();

    __mw_calltrace_size = sz;
    return ts;
}
//]
