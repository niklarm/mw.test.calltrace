//[timestamp_example
#include <mw/test/calltrace.hpp>
#include <chrono>

mw::test::timestamp_t mw::test::timestamp()
{
    return std::chrono::system_clock::now()
                .time_since_epoch().count();
}
//]
