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

namespace mw
{
namespace test
{

extern "C" {

#include <mw/test/calltrace.def>

}

template<std::size_t Size>
class calltrace : mw_calltrace_
{
    std::array<const void*, Size> _funcs;
    bool _inited;
public:
    template<typename Func, typename ...Args>
    inline calltrace(Func * func, int repeat, int skip, Args*... args) __attribute__((no_instrument_function));

    template<typename Func, typename ...Args>
    inline calltrace(Func * func, int repeat, Args*... args) __attribute__((no_instrument_function));

    template<typename Func, typename ...Args>
    inline calltrace(Func * func, Args*... args) __attribute__((no_instrument_function));

    inline bool inited  () const __attribute__((no_instrument_function));
    inline bool errored () const __attribute__((no_instrument_function));
    inline bool complete() const __attribute__((no_instrument_function));
    inline bool success () const __attribute__((no_instrument_function));
    inline operator bool() const __attribute__((no_instrument_function));

    ~calltrace()
    {
        __mw_reset_calltrace(this);
    }
};

template<std::size_t Size>
template<typename Func, typename ...Args>
calltrace<Size>::calltrace(Func * func, int repeat, int skip, Args*... args)
    : mw_calltrace_{reinterpret_cast<const void*>(func), _funcs.data(), _funcs.size(), repeat, skip, skip, 0, 0, 0, 0},
      _funcs({reinterpret_cast<const void*>(args)...}),
      _inited(__mw_set_calltrace(this) )
{

}

template<std::size_t Size>
template<typename Func, typename ...Args>
calltrace<Size>::calltrace(Func * func, int repeat, Args*... args)
            : calltrace(func, repeat, 0, args...) {}

template<std::size_t Size>
template<typename Func, typename ...Args>
calltrace<Size>::calltrace(Func * func, Args*... args)
            : calltrace(func, 0, 0, args...) {}


template<std::size_t Size> bool calltrace<Size>::inited  () const {return _inited;}
template<std::size_t Size> bool calltrace<Size>::errored () const {return mw_calltrace_::errored != 0;}
template<std::size_t Size> bool calltrace<Size>::complete() const {return (current_position == 0) && (repeated > 0);}
template<std::size_t Size> bool calltrace<Size>::success () const {return complete() && !errored() && inited();}
template<std::size_t Size> calltrace<Size>::operator bool() const {return success();}


template<typename Func, typename ...Args>
inline calltrace<sizeof...(Args)> make_calltrace(Func * func, int repeat, int skip, Args*... args) __attribute__((no_instrument_function));

template<typename Func, typename ...Args>
inline calltrace<sizeof...(Args)> make_calltrace(Func * func, int repeat, Args*... args) __attribute__((no_instrument_function));

template<typename Func, typename ...Args>
inline calltrace<sizeof...(Args)> make_calltrace(Func * func, Args*... args) __attribute__((no_instrument_function));


template<typename Func, typename ...Args>
inline calltrace<sizeof...(Args)> make_calltrace(Func * func, int repeat, int skip, Args*... args)
{
    return calltrace<sizeof...(Args)>(func, repeat, skip, args...);
}

template<typename Func, typename ...Args>
inline calltrace<sizeof...(Args)> make_calltrace(Func * func, int repeat, Args*... args)
{
    return calltrace<sizeof...(Args)>(func, repeat, args...);
}

template<typename Func, typename ...Args>
inline calltrace<sizeof...(Args)> make_calltrace(Func * func, Args*... args)
{
    return calltrace<sizeof...(Args)>(func, args...);
}


}
}


#endif /* MW_TEST_CALLTRACE_HPP_ */
