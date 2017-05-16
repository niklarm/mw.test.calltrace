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

#if !defined(MW_CALLTRACE_DOXYGEN)

#define MW_NO_INSTRUMENT __attribute__((no_instrument_function))

#endif

#if defined(MW_CALLTRACE_DOXYGEN)

///The function to be implemented if a timestamp shall be added to the function calls.
mw_timestamp_t mw_timestamp();

///The unsigned int type that is returned by the timestamp function.
typedef detail::mw_timestamp_t mw_timestamp_t;


/**The type carrying the necessary information for the calltrace.
 */
struct mw_calltrace
{
    ///The function which's subcalls are trace.
    const void * fn;

    ///The content, i.e. array of the calls
    const void ** content;
    ///The amount of functions traced
    const int content_size;

    ///How many function calls shall be traced
    const int repeat;
    ///How many function calls shall be ignored before tracing
    const int skip;
};

#else


typedef struct mw_calltrace_ mw_calltrace;

#endif

/**
 This function initializes and registers a calltrace.
 Note that there is a limited amount of calltraces that can be added. The default value is 16, but can be changed by defining
 `MW_CALLTRACE_STACK_SIZE` while compiling calltrace.

 \section mw_calltrace_init_example Example
 \code{.cpp}

 #include <cassert>
 #include <mw/test/calltrace.h>

 void func() {foo(); bar();}

 int main(int argc, char *argv[])
 {
    const void * ct_arr[] = {&foo, &bar};
    mw_calltrace ct = {&func, ct_arr, 2, 0, 0};

    assert(mw_calltrace_init(&ct));

    func();
    assert(mw_calltrace_success(&ct));
    assert(mw_calltrace_deinit(&ct));
    return 0;
 }
 \endcode

 \param ct A pointer to the calltrace to be initialized.
 \return A value differnt from zero if it succeeded.

 */
static int MW_NO_INSTRUMENT mw_calltrace_init(mw_calltrace * ct)
{
    ct->to_skip = ct->skip;
    ct->errored = 0;
    ct->repeated = 0;
    ct->current_position = 0;
    ct->start_depth = -1;

    return __mw_set_calltrace(ct);
}


/** This functions returns a value different from zero if the calltrace was completed.
 * The behaviour depends on the repeat setting of the calltrace.
 * If the calltrace is set to repeat n-times it has to be at least repeated once,
 * while any other number will require the calltrace to be repeated exactly as set.
 *
 * @param ct A pointer to the calltrace that shall be examined.
 * @return Unequal zero if successful.
 */
static int MW_NO_INSTRUMENT mw_calltrace_complete(mw_calltrace * ct)
{
    int rep_res = ct->repeat == 0 ? (ct->repeated > 0 ) : (ct->repeated >= ct->repeat);
    return (ct->current_position == 0) && rep_res;
}


/** This functions returns a value different from zero if the calltrace was completed and did not err.
 *
 * @param ct A pointer to the calltrace that shall be examined.
 * @return Unequal zero if completed without error.
 */
static int MW_NO_INSTRUMENT mw_calltrace_success (mw_calltrace * ct)
{
    return mw_calltrace_complete(ct) && !ct->errored;
}

/** This function deinitializes the calltrace and removes it from the stack.
 * @warning This function must be called before the calltrace struct is removed from the stack, otherwise it might casue memory leaks.
 *
 * @param ct The calltrace to be deinitialized
 * @return A value unequal to zero if the calltrace was succesful removed from the stack.
 */
static int MW_NO_INSTRUMENT mw_calltrace_deinit(mw_calltrace * ct)
{
    return __mw_reset_calltrace(ct);
}


#endif /* MW_TEST_CALLTRACE_H_ */
