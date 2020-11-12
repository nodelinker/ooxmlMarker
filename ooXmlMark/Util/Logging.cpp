#include "Logging.h"


void LoggingInit(){


	/* init boost log
	* 1. Add common attributes
	* 2. set log filter to trace
	*/
	boost::log::add_common_attributes();

	boost::log::core::get()->add_global_attribute("Scope",
		boost::log::attributes::named_scope());

	boost::log::core::get()->set_filter(
		boost::log::trivial::severity >= boost::log::trivial::trace
		);

	/* log formatter:
	* [TimeStamp] [ThreadId] [Severity Level] [Scope] Log message
	*/
	auto fmtTimeStamp = boost::log::expressions::
		format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
	auto fmtThreadId = boost::log::expressions::
		attr<boost::log::attributes::current_thread_id::value_type>("ThreadID");
	auto fmtSeverity = boost::log::expressions::
		attr<boost::log::trivial::severity_level>("Severity");
	auto fmtScope = boost::log::expressions::format_named_scope("Scope",
		boost::log::keywords::format = "%n(%f:%l)",
		boost::log::keywords::iteration = boost::log::expressions::reverse,
		boost::log::keywords::depth = 2);

	boost::log::formatter logFmt =
		boost::log::expressions::format("[%1%] (%2%) [%3%] [%4%] %5%")
		% fmtTimeStamp % fmtThreadId % fmtSeverity % fmtScope
		% boost::log::expressions::smessage;

	/* fs sink */
	auto fsSink = boost::log::add_file_log(
		boost::log::keywords::file_name = "simple.log",
		boost::log::keywords::rotation_size = 10 * 1024 * 1024,
		boost::log::keywords::min_free_space = 30 * 1024 * 1024,
		boost::log::keywords::open_mode = std::ios_base::app);

	fsSink->set_formatter(logFmt);
	fsSink->locked_backend()->auto_flush(true);

}
