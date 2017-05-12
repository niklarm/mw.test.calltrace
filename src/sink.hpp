/**
 * @file   /calltrace/src/sink.hpp
 * @date   08.05.2017
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
#ifndef SINK_HPP_
#define SINK_HPP_

#include <mw/debug/frame.hpp>
#include "calltrace_clone.hpp"

struct data_sink_t
{
    virtual void enter(const boost::optional<mw::debug::address_info>& func,
                       const boost::optional<mw::debug::address_info>& call_site,
                       const boost::optional<std::uint64_t> & ts = boost::none) = 0;
    virtual void exit (const boost::optional<mw::debug::address_info>& func,
                       const boost::optional<mw::debug::address_info>& call_site,
                       const boost::optional<std::uint64_t> & ts = boost::none) = 0;

    virtual void set  (const calltrace_clone & cc, const boost::optional<std::uint64_t> & ts) = 0;
    virtual void reset(const calltrace_clone & cc, int error_cnt, const boost::optional<std::uint64_t> & ts) = 0;

    virtual void overflow(const calltrace_clone & cc, const boost::optional<mw::debug::address_info> & ai) = 0;
    virtual void mismatch(const calltrace_clone & cc, const boost::optional<mw::debug::address_info> & ai) = 0;

    virtual void incomplete(const calltrace_clone & cc, int missing) = 0;
    virtual void timestamp_unavailable() = 0;


};

data_sink_t * get_hrf_sink (std::ostream & os);
data_sink_t * get_json_sink(std::ostream & os);


#endif /* SINK_HPP_ */
