/*
*          Copyright Andrey Semashev 2007 - 2015.
* Distributed under the Boost Software License, Version 1.0.
*    (See accompanying file LICENSE_1_0.txt or copy at
*          http://www.boost.org/LICENSE_1_0.txt)
*/


#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>


#define DEBUGPRINT printf

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

using namespace logging::trivial;

void LoggingInit();



// int test_logging()
// {
// 	LoggingInit();
// 	logging::add_common_attributes();

// 	using namespace logging::v;
// 	src::severity_logger< severity_level > lg;

	//bool is_all_lower = true;
	//std::string text = "aaaaaaaaaaaaa";
	//boost::format fmt = boost::format("<%s> %s in the lower case") % text % (is_all_lower ? "is" : "is not");
	//std::string output = fmt.str();

	//BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	//BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	//BOOST_LOG_TRIVIAL(info) << "An informational severity message";
	//BOOST_LOG_TRIVIAL(error) << "An error severity message";
	//BOOST_LOG_TRIVIAL(fatal) << "这些哈里斯的风景  是拉圣诞快乐房价阿克苏点击发送江东父老飒飒两地分居卡fg sss 111111111111123123";
	//BOOST_LOG_TRIVIAL(error) << output;
// 	return 0;
// }