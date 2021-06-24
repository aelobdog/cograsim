/*
 * Logger.h
 *
 *  Created on: 27/apr/2012
 *      Author: lorenzo
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include <iostream>
#include <cstdarg>
#include <cstdio>

namespace Logging {
	extern bool debug;
	void log(std::string msg, ...);
	void log_debug(std::string msg, ...);
	void log_info(std::string msg, ...);
	void log_warning(std::string msg, ...);
	void log_critical(std::string msg, ...);
	void _log(std::string descr, std::string msg, va_list &ap);
}

#endif /* LOGGING_H_ */
