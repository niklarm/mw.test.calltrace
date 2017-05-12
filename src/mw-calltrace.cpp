/**
 * @file   /gdb-runner/src/mw-test-backend.cpp
 * @date   01.08.2016
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

#include <boost/dll/alias.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/system/api_config.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/program_options.hpp>
#include <mw/debug/break_point.hpp>
#include <mw/debug/frame.hpp>
#include <mw/debug/plugin.hpp>

#include <unordered_map>

#include <iostream>
#include <fstream>

#include "sink.hpp"

using namespace mw::debug;
using namespace std;

data_sink_t * data_sink = nullptr;

std::string sink_file;
std::string format;
bool log_all = false;
bool profile = false;
bool minimal = false;
int ct_depth = -1;



struct mw_calltrace : break_point
{
    std::unordered_map<std::uint64_t, boost::optional<address_info>> addr_map;

    std::vector<calltrace_clone> cts;

    bool timestamp_available = true;

    //buffered version
    boost::optional<address_info> addr2line(frame & fr, std::uint64_t pos)
    {
        auto itr = addr_map.find(pos);
        if (itr != addr_map.end())
            return itr->second;
        else
            return addr_map.emplace(pos, fr.addr2line(pos)).first->second;
    }

    mw_calltrace() : break_point("__mw_profile")
    {
    }

    void invoke(frame & fr, const string & file, int line) override
    {
        auto arg = fr.arg_list(0);
        if (arg.value == "mw_enter")
            enter(fr);
        else if (arg.value == "mw_exit")
            exit(fr);
        else if (arg.value == "mw_set")
            set(fr);
        else if (arg.value == "mw_reset")
            reset(fr);
    }


    void enter(frame & fr)
    {
        auto function_arg = fr.arg_list(1);
        auto callsite_arg = fr.arg_list(2);

        auto function_ptr = std::stoull(function_arg.value, nullptr, 16);
        auto callsite_ptr = std::stoull(callsite_arg.value, nullptr, 16);

        auto function_loc = addr2line(fr, function_ptr);
        auto callsite_loc = addr2line(fr, callsite_ptr);

        auto ts = timestamp(fr);

        if (!minimal)
            data_sink->enter(function_loc, callsite_loc, ts);

        if (cts.empty())
            return;

        int depth = std::stoi(fr.print("__mw_calltrace_depth").value);

        for (auto & ct : cts)
        {
            if (ct.inactive() &&
               (ct.fn().address == function_ptr) &&
               ((ct.repeat() > ct.repeated()) || (ct.repeat() == 0)))
            {
                if (ct.to_skip())
                    ct.add_skipped();
                else
                    ct.start(depth);
            }
            else if (ct.my_child(depth))
            {
                if (!ct.check(function_ptr))
                {
                    if (ct.ovl())
                        //log the overflow
                        data_sink->overflow(ct, function_loc);
                    else
                        //log the error
                        data_sink->mismatch(ct, function_loc);
                }
            }
        }
    }
    void exit(frame & fr)
    {
        auto function_arg = fr.arg_list(1);
        auto callsite_arg = fr.arg_list(2);
        auto function_loc = addr2line(fr, std::stoull(function_arg.value, nullptr, 16));
        auto callsite_loc = addr2line(fr, std::stoull(callsite_arg.value, nullptr, 16));

        auto ts = timestamp(fr);
        if (!minimal)
            data_sink->exit(function_loc, callsite_loc, ts);

        if (cts.empty())
            return;

        int depth = std::stoi(fr.print("__mw_calltrace_depth").value);

        for (auto & ct : cts)
        {
            if (ct.my_depth(depth))
            {
                int pos = ct.current_position();
                if (!ct.stop()) //log the incomplete
                    data_sink->incomplete(ct, pos);
            }
        }
    }
    void set(frame & fr)
    {
        auto ct_ptr = fr.arg_list(1).value;
        auto ct_str = "((struct mw_calltrace_*)"+ ct_ptr +")";

        calltrace_clone_entry fn;
        fn.address = std::stoull(fr.print(ct_str + "->fn").value, nullptr, 16);
        fn.info    = addr2line(fr, fn.address);

        auto sz = std::stoull(fr.print(ct_str + "->content_size").value);
        std::vector<calltrace_clone_entry> content;
        content.reserve(sz);

        for (auto i = 0u; i<sz; i++)
        {
            auto p = std::stoull(fr.print(ct_str + "->content[" + std::to_string(i) + "]").value, nullptr, 16);
            if (p != 0)
            {
                auto fn = addr2line(fr, p);
                content.emplace_back(p, std::move(fn));
            }
            else
                content.emplace_back(p, boost::none);
        }

        auto repeat = std::stoi(fr.print(ct_str + "->repeat").value);
        auto skip   = std::stoi(fr.print(ct_str + "->skip").value);
        cts.emplace_back(std::stoull(ct_ptr, nullptr, 16), std::move(fn), std::move(content), repeat, skip);
        data_sink->set(cts.back(), timestamp(fr));

    }
    void reset(frame & fr)
    {
        auto ct_ptr = std::stoull(fr.arg_list(1).value, nullptr, 16);

        auto itr = cts.begin();
        while ((itr != std::find_if(itr, cts.end(), [&](const calltrace_clone & cc){return cc.location() == ct_ptr;})))
        {
            data_sink->reset(*itr, itr->errors(), timestamp(fr));
            cts.erase(itr);
        }
    }

    boost::optional<std::uint64_t> timestamp(frame &fr)
    {
        if (!profile || !timestamp_available)
            return boost::none;



        fr.disable(*this);
        auto ct_size = fr.print("__mw_calltrace_size").value;
        fr.set("__mw_calltrace_size", "0");

        std::string value;
        try {
            auto res = fr.print("mw_timestamp()");
            value = res.value;
        }
        catch (mw::debug::interpreter_error & ie) //that means it's not available.
        {
            data_sink->timestamp_unavailable();
            timestamp_available = false;
        }
        fr.set("__mw_calltrace_size", ct_size);
        fr.enable(*this);

        if (value.empty())
            return boost::none;
        else
            return std::stoull(value);
    }

};



boost::optional<std::ofstream> fstr;

std::ostream * sink_str = & std::cout;


void mw_dbg_setup_bps(vector<unique_ptr<mw::debug::break_point>> & bps)
{
    if (!sink_file.empty())
    {
        fstr.emplace(sink_file);
        sink_str = &*fstr;
    }
    //ok, we setup the logger
    if (format.empty() || (format == "hrf"))
        data_sink = get_hrf_sink(*sink_str);
    else if (format == "json")
        data_sink = get_json_sink(*sink_str);
    else
        std::cerr << "Unknown format \"" << format << "\"" << std::endl;

    bps.push_back(make_unique<mw_calltrace>());
    if (!log_all || (ct_depth >= 0)) //conditions
    {
        std::string condition;
        if (ct_depth >= 0)
            condition = "ct_depth <= " + std::to_string(ct_depth);

        if (!log_all)
        {
            if (condition.empty())
                condition = "__mw_calltrace_size > 0";
            else
                condition = "(" + condition + ") &&  (__mw_calltrace_size > 0)";
        }
        bps.back()->set_condition("__mw_calltrace_size > 0");
    }


}


void mw_dbg_setup_options(boost::program_options::options_description & op)
{
    namespace po = boost::program_options;
    op.add_options()
                   ("mw-calltrace-sink",      po::value<string>(&sink_file),  "test data sink")
                   ("mw-calltrace-format",    po::value<string>(&format),     "format [hrf, json]")
                   ("mw-calltrace-all",       po::bool_switch(&log_all),      "log all calls")
                   ("mw-calltrace-timestamp", po::bool_switch(&profile),      "enable profiling")
                   ("mw-calltrace-minimal",   po::bool_switch(&minimal),      "only output the result of the actual calltraces")
                   ("mw-calltrace-depth",     po::value<int>(&ct_depth)->default_value(-1), "maximum depth of the calltrace recording")
                   ;
}
