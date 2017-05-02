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

#include <mw/test/calltrace.def>

typedef mw_calltrace_ mw_calltrace;

static int mw_init_calltrace(mw_calltrace * ct)
{
    ct->to_skip = ct->skip;
    ct->errored = 0;
    ct->repeated = 0;
    ct->current_position = 0;
    ct->start_depth = 0;
    return __mw_set_calltrace(ct);
}

static int mw_complete(mw_calltrace * ct)
{
    return ct->current_position == 0;
}
static int mw_success (mw_calltrace * ct) {return mw_complete(ct) && !ct->errored;}

static int mw_deinit_calltrace(mw_calltrace * ct)
{
    return __mw_reset_calltrace(ct);
}


#endif /* MW_TEST_CALLTRACE_H_ */
