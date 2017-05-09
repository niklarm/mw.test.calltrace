/**
 * @file   mw/test/calltrace.h
 * @date   02.05.2017
 * @author Klemens D. Morgenstern
 *
 * Published under [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0.html)
  <pre>
    /  /|  (  )   |  |  /
   /| / |   \/    | /| /
  / |/  |  / \    |/ |/
 /  /   | (   \   /  |
               )
 </pre>

 This header provides the C version of the test macros.
 */
#ifndef MW_TEST_CALLTRACE_H_
#define MW_TEST_CALLTRACE_H_

#include <limits.h>
#include <mw/test/calltrace.def>



#define MW_NO_INSTRUMENT __attribute__((no_instrument_function))

typedef struct mw_calltrace_ mw_calltrace;

static int MW_NO_INSTRUMENT mw_calltrace_init(mw_calltrace * ct)
{
    ct->to_skip = ct->skip;
    ct->errored = 0;
    ct->repeated = 0;
    ct->current_position = 0;
    ct->start_depth = -1;

    return __mw_set_calltrace(ct);
}

static int MW_NO_INSTRUMENT mw_calltrace_complete(mw_calltrace * ct)
{
    int rep_res = ct->repeat == 0 ? (ct->repeated > 0 ) : (ct->repeated >= ct->repeat);
    return (ct->current_position == 0) && rep_res;
}

static int MW_NO_INSTRUMENT mw_calltrace_success (mw_calltrace * ct)
{
    return mw_calltrace_complete(ct) && !ct->errored;
}

static int MW_NO_INSTRUMENT mw_calltrace_deinit(mw_calltrace * ct)
{
    return __mw_reset_calltrace(ct);
}


#endif /* MW_TEST_CALLTRACE_H_ */
