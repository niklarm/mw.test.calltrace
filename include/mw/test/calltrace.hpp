/**
 * @file   mw/test/calltrace.hpp
 * @date   29.07.2016
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

 This header provides the C++ version of the call trace definitions.
 */
#ifndef MW_TEST_CALLTRACE_HPP_
#define MW_TEST_CALLTRACE_HPP_

#include <array>
#include <iostream>
#include <climits>

#define MW_NO_INSTRUMENT __attribute__((no_instrument_function))

namespace mw
{
namespace test
{

extern "C" {

#include <mw/test/calltrace.def>

}

#pragma GCC diagnostic ignored "-Wpmf-conversions"


struct any_fn_t
{
    constexpr any_fn_t() MW_NO_INSTRUMENT {};

    constexpr MW_NO_INSTRUMENT operator const void* () const {return nullptr;}
};

constexpr static any_fn_t any_fn;


template<typename T> struct fn;

template<typename Return, typename ... Args>
struct fn<Return(Args...)>
{
    const void* ptr;

    MW_NO_INSTRUMENT fn(Return (*ptr)(Args...)) : ptr(reinterpret_cast<const void*>(ptr)) {}

    template<typename Class>
    MW_NO_INSTRUMENT fn(Return (Class::*ptr)(Args...)) : ptr(reinterpret_cast<const void*>(ptr)) {}

    template<typename Class>
    MW_NO_INSTRUMENT fn(Return (Class::*ptr)(Args...) const )  : ptr(reinterpret_cast<const void*>(ptr)) {}

    template<typename Class>
    MW_NO_INSTRUMENT fn(Return (Class::*ptr)(Args...) volatile ) : ptr(reinterpret_cast<const void*>(ptr)) {}

    template<typename Class>
    MW_NO_INSTRUMENT fn(Return (Class::*ptr)(Args...) const volatile ) : ptr(reinterpret_cast<const void*>(ptr)) {}

    template<typename Class>
    MW_NO_INSTRUMENT fn(Return (Class::*ptr)(Args...) &) : ptr(reinterpret_cast<const void*>(ptr)) {}

    template<typename Class>
    MW_NO_INSTRUMENT fn(Return (Class::*ptr)(Args...) const &) : ptr(reinterpret_cast<const void*>(ptr)) {}

    template<typename Class>
    MW_NO_INSTRUMENT fn(Return (Class::*ptr)(Args...) volatile &) : ptr(reinterpret_cast<const void*>(ptr)) {}

    template<typename Class>
    MW_NO_INSTRUMENT fn(Return (Class::*ptr)(Args...) const volatile &) : ptr(reinterpret_cast<const void*>(ptr)) {}

    template<typename Class>
    MW_NO_INSTRUMENT fn(Return (Class::*ptr)(Args...) &&) : ptr(reinterpret_cast<const void*>(ptr)) {}

    template<typename Class>
    MW_NO_INSTRUMENT fn(Return (Class::*ptr)(Args...) const &&) : ptr(reinterpret_cast<const void*>(ptr)) {}

    template<typename Class>
    MW_NO_INSTRUMENT fn(Return (Class::*ptr)(Args...) volatile &&) : ptr(reinterpret_cast<const void*>(ptr)) {}

    template<typename Class>
    MW_NO_INSTRUMENT fn(Return (Class::*ptr)(Args...) const volatile &&) : ptr(reinterpret_cast<const void*>(ptr)) {}

};

template<typename = void>
struct mem_fn;

template<>
struct mem_fn<void>
{
    const void * ptr;

    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn(Return(Class::*ptr)(Args...)) : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn(Return(Class::*ptr)(Args...) &) : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn(Return(Class::*ptr)(Args...) &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};


template<typename Return, typename ... Args>
struct mem_fn<Return(Args...)>
{
    const void * ptr;

    template<typename Class>
    MW_NO_INSTRUMENT mem_fn(Return(Class::*ptr)(Args...))    : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Class>
    MW_NO_INSTRUMENT mem_fn(Return(Class::*ptr)(Args...) &)  : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Class>
    MW_NO_INSTRUMENT mem_fn(Return(Class::*ptr)(Args...) &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};

template<typename = void>
struct mem_fn_c;

template<>
struct mem_fn_c<void>
{
    const void * ptr;

    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_c(Return(Class::*ptr)(Args...) const) : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_c(Return(Class::*ptr)(Args...) const &) : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_c(Return(Class::*ptr)(Args...) const &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};


template<typename Return, typename ... Args>
struct mem_fn_c<Return(Args...)>
{
    const void * ptr;

    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_c(Return(Class::*ptr)(Args...) const) : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_c(Return(Class::*ptr)(Args...) const &) : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_c(Return(Class::*ptr)(Args...) const &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};

template<typename = void>
struct mem_fn_v;

template<>
struct mem_fn_v<void>
{
    const void * ptr;

    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_v(Return(Class::*ptr)(Args...) volatile) : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_v(Return(Class::*ptr)(Args...) volatile &) : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_v(Return(Class::*ptr)(Args...) volatile &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};


template<typename Return, typename ... Args>
struct mem_fn_v<Return(Args...)>
{
    const void * ptr;

    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_v(Return(Class::*ptr)(Args...) volatile)    : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_v(Return(Class::*ptr)(Args...) volatile &)  : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_v(Return(Class::*ptr)(Args...) volatile &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};

template<typename = void>
struct mem_fn_cv;

template<>
struct mem_fn_cv<void>
{
    const void * ptr;

    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_cv(Return(Class::*ptr)(Args...) const volatile)    : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_cv(Return(Class::*ptr)(Args...) const volatile &)  : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_cv(Return(Class::*ptr)(Args...) const volatile &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};


template<typename Return, typename ... Args>
struct mem_fn_cv<Return(Args...)>
{
    const void * ptr;

    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_cv(Return(Class::*ptr)(Args...) const volatile)    : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_cv(Return(Class::*ptr)(Args...) const volatile &)  : ptr(reinterpret_cast<const void*>(ptr)) {}
    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_cv(Return(Class::*ptr)(Args...) const volatile &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};

template<typename = void>
struct mem_fn_lvalue;

template<>
struct mem_fn_lvalue<void>
{
    const void * ptr;

    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_lvalue(Return(Class::*ptr)(Args...) &) : ptr(reinterpret_cast<const void*>(ptr)) {}
};


template<typename Return, typename ... Args>
struct mem_fn_lvalue<Return(Args...)>
{
    const void * ptr;

    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_lvalue(Return(Class::*ptr)(Args...) &) : ptr(reinterpret_cast<const void*>(ptr)) {}
};

template<typename = void>
struct mem_fn_c_lvalue;

template<>
struct mem_fn_c_lvalue<void>
{
    const void * ptr;

    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_c_lvalue(Return(Class::*ptr)(Args...) const &) : ptr(reinterpret_cast<const void*>(ptr)) {}
};


template<typename Return, typename ... Args>
struct mem_fn_c_lvalue<Return(Args...)>
{
    const void * ptr;

    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_c_lvalue(Return(Class::*ptr)(Args...) const &) : ptr(reinterpret_cast<const void*>(ptr)) {}
};

template<typename = void>
struct mem_fn_v_lvalue;

template<>
struct mem_fn_v_lvalue<void>
{
    const void * ptr;

    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_v_lvalue(Return(Class::*ptr)(Args...) volatile &) : ptr(reinterpret_cast<const void*>(ptr)) {}
};


template<typename Return, typename ... Args>
struct mem_fn_v_lvalue<Return(Args...)>
{
    const void * ptr;

    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_v_lvalue(Return(Class::*ptr)(Args...) volatile &) : ptr(reinterpret_cast<const void*>(ptr)) {}
};

template<typename = void>
struct mem_fn_cv_lvalue;

template<>
struct mem_fn_cv_lvalue<void>
{
    const void * ptr;

    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_cv_lvalue(Return(Class::*ptr)(Args...) const volatile &) : ptr(reinterpret_cast<const void*>(ptr)) {}
};


template<typename Return, typename ... Args>
struct mem_fn_cv_lvalue<Return(Args...)>
{
    const void * ptr;

    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_cv_lvalue(Return(Class::*ptr)(Args...) const volatile &) : ptr(reinterpret_cast<const void*>(ptr)) {}
};

template<typename = void>
struct mem_fn_rvalue;

template<>
struct mem_fn_rvalue<void>
{
    const void * ptr;

    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_rvalue(Return(Class::*ptr)(Args...)) : ptr(reinterpret_cast<const void*>(ptr)) {}
};


template<typename Return, typename ... Args>
struct mem_fn_rvalue<Return(Args...)>
{
    const void * ptr;

    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_rvalue(Return(Class::*ptr)(Args...) &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};

template<typename = void>
struct mem_fn_c_rvalue;

template<>
struct mem_fn_c_rvalue<void>
{
    const void * ptr;

    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_c_rvalue(Return(Class::*ptr)(Args...) const &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};


template<typename Return, typename ... Args>
struct mem_fn_c_rvalue<Return(Args...)>
{
    const void * ptr;

    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_c_rvalue(Return(Class::*ptr)(Args...) const &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};

template<typename = void>
struct mem_fn_v_rvalue;

template<>
struct mem_fn_v_rvalue<void>
{
    const void * ptr;

    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_v_rvalue(Return(Class::*ptr)(Args...) volatile &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};


template<typename Return, typename ... Args>
struct mem_fn_v_rvalue<Return(Args...)>
{
    const void * ptr;

    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_v_rvalue(Return(Class::*ptr)(Args...) volatile &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};

template<typename = void>
struct mem_fn_cv_rvalue;

template<>
struct mem_fn_cv_rvalue<void>
{
    const void * ptr;

    template<typename Return, typename Class, typename ...Args>
    MW_NO_INSTRUMENT mem_fn_cv_rvalue(Return(Class::*ptr)(Args...) const volatile &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};


template<typename Return, typename ... Args>
struct mem_fn_cv_rvalue<Return(Args...)>
{
    const void * ptr;

    template<typename Class>
    MW_NO_INSTRUMENT mem_fn_cv_rvalue(Return(Class::*ptr)(Args...) const volatile &&) : ptr(reinterpret_cast<const void*>(ptr)) {}
};

namespace detail
{
    template<typename Return, typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(*t)(Args...))
    {
        return reinterpret_cast<const void*>(t);
    }

    template<typename Return, typename Class,  typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(Class::*t)(Args...))
    {
        return reinterpret_cast<const void*>(t);
    }

    template<typename Return, typename Class,  typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(Class::*t)(Args...) const)
    {
        return reinterpret_cast<const void*>(t);
    }

    template<typename Return, typename Class,  typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(Class::*t)(Args...) volatile )
    {
        return reinterpret_cast<const void*>(t);
    }

    template<typename Return, typename Class,  typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(Class::*t)(Args...) const volatile)
    {
        return reinterpret_cast<const void*>(t);
    }

    template<typename Return, typename Class,  typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(Class::*t)(Args...) &)
    {
        return reinterpret_cast<const void*>(t);
    }


    template<typename Return, typename Class,  typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(Class::*t)(Args...) const &)
    {
        return reinterpret_cast<const void*>(t);
    }

    template<typename Return, typename Class,  typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(Class::*t)(Args...) volatile &)
    {
        return reinterpret_cast<const void*>(t);
    }

    template<typename Return, typename Class,  typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(Class::*t)(Args...) const volatile &)
    {
        return reinterpret_cast<const void*>(t);
    }


    template<typename Return, typename Class,  typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(Class::*t)(Args...) &&)
    {
        return reinterpret_cast<const void*>(t);
    }


    template<typename Return, typename Class,  typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(Class::*t)(Args...) const &&)
    {
        return reinterpret_cast<const void*>(t);
    }

    template<typename Return, typename Class,  typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(Class::*t)(Args...) volatile &&)
    {
        return reinterpret_cast<const void*>(t);
    }

    template<typename Return, typename Class,  typename ...Args>
    inline const void * MW_NO_INSTRUMENT func_cast(Return(Class::*t)(Args...) const volatile &&)
    {
        return reinterpret_cast<const void*>(t);
    }

    inline const void * MW_NO_INSTRUMENT func_cast(const any_fn_t &t)
    {
        return nullptr;
    }

    template<typename T>
    inline const void * MW_NO_INSTRUMENT func_cast(const fn<T> &t)
    {
        return t.ptr;
    }

    template<typename T>
    inline const void * MW_NO_INSTRUMENT func_cast(const mem_fn<T> &t)
    {
        return t.ptr;
    }

    template<typename T>
    inline const void * MW_NO_INSTRUMENT func_cast(const mem_fn_c<T> &t)
    {
        return t.ptr;
    }
    template<typename T>
    inline const void * MW_NO_INSTRUMENT func_cast(const mem_fn_v<T> &t)
    {
        return t.ptr;
    }

    template<typename T>
    inline  const void * MW_NO_INSTRUMENT func_cast(const mem_fn_cv<T> &t)
    {
        return t.ptr;
    }

    template<typename T>
    inline const void * MW_NO_INSTRUMENT func_cast(const mem_fn_lvalue<T> &t)
    {
        return t.ptr;
    }

    template<typename T>
    inline const void * MW_NO_INSTRUMENT func_cast(const mem_fn_c_lvalue<T> &t)
    {
        return t.ptr;
    }
    template<typename T>
    inline const void * MW_NO_INSTRUMENT func_cast(const mem_fn_v_lvalue<T> &t)
    {
        return t.ptr;
    }

    template<typename T>
    inline const void * MW_NO_INSTRUMENT func_cast(const mem_fn_cv_lvalue<T> &t)
    {
        return t.ptr;
    }

    template<typename T>
    inline const void * MW_NO_INSTRUMENT func_cast(const mem_fn_rvalue<T> &t)
    {
        return t.ptr;
    }
    template<typename T>
    inline const void * MW_NO_INSTRUMENT func_cast(const mem_fn_c_rvalue<T> &t)
    {
        return t.ptr;
    }
    template<typename T>
    inline const void * MW_NO_INSTRUMENT func_cast(const mem_fn_v_rvalue<T> &t)
    {
        return t.ptr;
    }

    template<typename T>
    inline const void * MW_NO_INSTRUMENT func_cast(const mem_fn_cv_rvalue<T> &t)
    {
        return t.ptr;
    }

}

template<std::size_t Size>
class calltrace : mw_calltrace_
{
    const void*  _funcs[Size];
    bool _inited;
public:
    template<typename Func, typename ...Args>
    inline MW_NO_INSTRUMENT calltrace(Func func, int repeat, int skip, Args ... args)
        : mw_calltrace_{detail::func_cast(func), _funcs, static_cast<int>(Size), repeat, skip, skip, 0, 0, 0, -1},
          _funcs{detail::func_cast(args)...},
          _inited{__mw_set_calltrace(this) != 0}
    {
    }

    calltrace(const calltrace &) = delete;

    template<typename Func, typename ...Args>
    inline MW_NO_INSTRUMENT calltrace(Func func, int repeat, Args ... args) : calltrace(func, repeat, 0, args...) {}


    template<typename Func, typename ...Args>
    inline MW_NO_INSTRUMENT calltrace(Func func, Args ... args) : calltrace(func, 0, 0, args...) {};

    inline bool MW_NO_INSTRUMENT inited  () const {return _inited;}
    inline bool MW_NO_INSTRUMENT errored () const {return mw_calltrace_::errored != 0;}
    inline bool MW_NO_INSTRUMENT complete() const
    {
        bool rep_res = repeat == 0 ? (repeated > 0) : (repeated >= repeat);
        return (current_position == 0) && rep_res;
    }
    inline bool MW_NO_INSTRUMENT success () const {return complete() && !errored() && inited();}
    inline MW_NO_INSTRUMENT operator bool() const {return success();}

    MW_NO_INSTRUMENT ~calltrace()
    {
        __mw_reset_calltrace(this);
    }
};

#pragma GCC diagnostic pop

}
}


#endif /* MW_TEST_CALLTRACE_HPP_ */
