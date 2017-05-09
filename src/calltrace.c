/**
 * @file   calltrace.c
 * @date   01.05.2017
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

#include <stdint.h>
#include <mw/test/calltrace.def>

enum type_t
{
    mw_enter, mw_exit, mw_set, mw_reset
};

void __mw_profile(enum type_t type, void* this_fn, void *call_site) __attribute__((no_instrument_function, noinline));
void __mw_profile(enum type_t type, void* this_fn, void *call_site)
{
    //for the bp
}

#if defined(ULLONG_MAX)
typedef unsigned long long timestamp_t;
#else
typedef unsigned long timestamp_t;
#endif

mw_timestamp_t __attribute__((weak)) mw_timestamp()
{
    return 0;
}


#if !defined(MW_CALLTRACE_STACK_SIZE)
#define MW_CALLTRACE_STACK_SIZE 16
#endif


static struct mw_calltrace_ * calltrace_stack[MW_CALLTRACE_STACK_SIZE];
int __mw_calltrace_size = -1;


unsigned int __mw_calltrace_depth = 1;
void __mw_calltrace_enter(void * this_fn) __attribute__((no_instrument_function));
void __mw_calltrace_exit (void * this_fn) __attribute__((no_instrument_function));


void __mw_calltrace_enter(void * this_fn)
{
    int i, cnt = 0;
    for (i = 0; (cnt < __mw_calltrace_size) && (i <= sizeof(calltrace_stack)/sizeof(struct mw_calltrace_ * )); i++)
    {
        if (calltrace_stack[i] != 0)
        {
            cnt++;
            struct mw_calltrace_ *ct = calltrace_stack[i];

            if ((ct->start_depth == -1) //inactive
             && (ct->fn == this_fn) //that's my function
             && ((ct->repeat > ct->repeated) || (ct->repeat == 0) )) //not repeated to often
            {
                if (ct->to_skip > 0)
                {
                    ct->to_skip--;
                    continue;
                }
                else
                {
                    ct->start_depth = __mw_calltrace_depth;
                }
            }
            else if (ct->start_depth == (__mw_calltrace_depth-1)) //alright, I'm on the spot
            {
                if (ct->current_position >= ct->content_size) //to many calls
                {
                    ct->errored = 1;
                    continue;
                }
                const void * ptr = ct->content[ct->current_position];
                if ((ptr != this_fn) && (ptr != 0))
                    ct->errored = 1;
                ct->current_position++;
            }
        }
    }

}

void __mw_calltrace_exit (void * this_fn)
{
    int i, cnt = 0;
    for (i = 0; (cnt < __mw_calltrace_size) && (i <= sizeof(calltrace_stack)/sizeof(struct mw_calltrace_ * )); i++)
    {
        if (calltrace_stack[i] != 0)
        {
            cnt++;
            struct mw_calltrace_ *ct = calltrace_stack[i];
            if (ct->start_depth == __mw_calltrace_depth)
            {
                ct->start_depth = -1;

                if (ct->current_position != ct->content_size)
                    ct->errored = 1;

                ct->current_position = 0;
                ct->repeated ++;
            }
        }
    }
}


int __mw_set_calltrace(struct mw_calltrace_ * ct) __attribute__((no_instrument_function));
int __mw_set_calltrace(struct mw_calltrace_ * ct)
{
    if (__mw_calltrace_size < 0) //init the array first
    {
        int i;
        for (i = 0; i < sizeof(calltrace_stack)/sizeof(struct mw_calltrace_ *); i++)
            calltrace_stack[i] = 0;
        __mw_calltrace_size = 0;
    }

    if (__mw_calltrace_size >= (sizeof(calltrace_stack)/sizeof(struct mw_calltrace_ *)))
        return 0;

    int i;
    for (i = 0; i < (sizeof(calltrace_stack)/sizeof(struct mw_calltrace_ *)); i++)
    {
        if (calltrace_stack[i] == 0)
        {
            calltrace_stack[i] = ct;
            __mw_calltrace_size ++;
            __mw_profile(mw_set, ct, 0);
            break;
        }
    }
    return 1;
}

int __mw_reset_calltrace(struct mw_calltrace_ * ct) __attribute__((no_instrument_function));
int __mw_reset_calltrace(struct mw_calltrace_ * ct)
{
    int i;
    for (i = 0; i < (sizeof(calltrace_stack)/sizeof(struct mw_calltrace_ *)); i++)
    {
        if (calltrace_stack[i] == ct)
        {
            calltrace_stack[i] = 0;
            __mw_profile(mw_reset, ct, 0);
            __mw_calltrace_size --;
            return 1;
        }
    }
    return 0;
}


void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__((no_instrument_function));
void __cyg_profile_func_exit (void *this_fn, void *call_site) __attribute__((no_instrument_function));

void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
    __mw_calltrace_depth++;

    __mw_profile(mw_enter, this_fn, call_site);

    if (__mw_calltrace_size > 0)
        __mw_calltrace_enter(this_fn);
}
void __cyg_profile_func_exit (void *this_fn, void *call_site)
{
    __mw_profile(mw_exit, this_fn, call_site);
    if (__mw_calltrace_size > 0)
        __mw_calltrace_exit(this_fn);

    __mw_calltrace_depth--;
}
