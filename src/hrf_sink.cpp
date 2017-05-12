/**
 * @file   sink.cpp
 * @date   12.09.2016
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
#include "sink.hpp"
#include <iostream>
#include <boost/optional.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <sstream>

using namespace std;

struct hrf_sink_t : data_sink_t
{
    std::ostream * os = &std::cout;

    std::string group{"__mw_call"};

    std::ostream & loc(const std::string & file, int line)
    {
        return *os << boost::replace_all_copy(file, "\\\\", "\\") << '(' << line << ')';
    }

    virtual ~hrf_sink_t() = default;


    std::string loc(const boost::optional<mw::debug::address_info> & location)
    {
        if (!location)
            return "***unknown location***(0)";
        else if (!location->full_name)
            return boost::replace_all_copy(location->file, "\\\\", "\\") + "(" + std::to_string(location->line) + ")";
        else
            return boost::replace_all_copy(*location->full_name, "\\\\", "\\") + "(" + std::to_string(location->line) + ")";
    }

    std::string fn(std::uint64_t ptr, const boost::optional<mw::debug::address_info> & func)
    {
        std::stringstream ss;
        if (!func || !func->function)
            ss << "@0x" << std::hex << ptr << ":***unknown function***";
        else
            ss << "@0x" << std::hex << ptr << ":" << *func->function;

        return ss.str();
    }

    void enter(std::uint64_t      func_ptr, const boost::optional<mw::debug::address_info> & func,
               std::uint64_t call_site_ptr, const boost::optional<mw::debug::address_info>& call_site,
               const boost::optional<std::uint64_t> & ts) override
    {
        *os << "mw.calltrace entering function [" << fn(func_ptr, func) << "]";
        if (ts)
            *os << ", with timestamp " << *ts;

        if (call_site)
            *os << ", at "  << loc(call_site);
        *os << std::endl;
    }
    void exit (std::uint64_t      func_ptr, const boost::optional<mw::debug::address_info> & func,
               std::uint64_t call_site_ptr, const boost::optional<mw::debug::address_info>& call_site,
               const boost::optional<std::uint64_t> & ts) override
    {
        *os << "mw.calltrace  exiting function [" << fn(func_ptr, func) << "]";
        if (ts)
            *os << ", with timestamp " << *ts;

        if (call_site)
            *os << ", at "  << loc(call_site);
        *os << std::endl;
    }

    void set(const calltrace_clone & cc, const boost::optional<std::uint64_t> & ts) override
    {
        *os << "mw.calltrace   registered calltrace @0x" << std::hex << cc.location() << std::dec <<" [" << fn(cc.fn().address, cc.fn().info) <<  "]: ";

        *os << "{";

        for (const auto & val : cc.content())
        {
            if (&val != &cc.content().front())
                *os << ", ";

            if (val.address == 0ull)
                *os << "**any**";
            else
                *os << "[" << fn(val.address, val.info) << "]";
        }
        *os << "}";

        if (ts)
            *os << ", with timestamp " << *ts;

        *os << std::endl;
    }

    void reset(const calltrace_clone & cc, int error_cnt, const boost::optional<std::uint64_t> & ts) override
    {
        *os << "mw.calltrace unregistered calltrace @0x" << std::hex << cc.location() << std::dec
            << " executed " << cc.repeated() << " times, with " << error_cnt << " errors";
        if (ts)
            *os << ", with timestamp " << *ts;
        *os << std::endl;
    }

    void overflow(const calltrace_clone & cc, std::uint64_t addr, const boost::optional<mw::debug::address_info> & ai) override
    {
        *os << "mw.calltrace.error overflow in calltrace @0x" << std::hex << cc.location()
            << std::dec << " with size " << cc.content().size() << " at [" << fn(addr, ai) << "]" << std::endl;
    }
    void mismatch(const calltrace_clone & cc, std::uint64_t addr, const boost::optional<mw::debug::address_info> & ai) override
    {
         *os << "mw.calltrace.error mismatch in calltrace @0x" << std::hex << cc.location()
             << std::dec << " {[" << fn(addr, ai) << "] != [" << fn(cc.fn().address, cc.fn().info) << "]}" << std::endl;
    }
    void incomplete(const calltrace_clone & cc, int position) override
    {
        auto sz = cc.content().size();
        *os << "mw.calltrace.error incomplete in calltrace @0x" << std::hex << cc.location() << std::dec
            << " stopped " << position << "/" << sz << std::endl;
    }

    void timestamp_unavailable() override
    {
        *os << "mw.calltrace.error timestamp not available" << std::endl;
    }
};

boost::optional<hrf_sink_t> hrf_sink;

data_sink_t * get_hrf_sink(std::ostream & os)
{
    hrf_sink.emplace();
    hrf_sink->os = &os;
    return &*hrf_sink;
}
