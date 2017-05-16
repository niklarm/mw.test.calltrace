//[timestamp_example_c
#include <mw/test/calltrace.h>
#include <time.h>

mw_timestamp_t mw_timestamp()
{
    return (mw_timestamp_t)time(0);
}

//]
