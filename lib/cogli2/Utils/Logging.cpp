/*
 * Logger.cpp
 *
 *  Created on: 27/apr/2012
 *      Author: lorenzo
 */

#include "Logging.h"

bool Logging::debug = false;

void Logging::log(std::string msg, ...) {
	va_list ap;
	va_start(ap, msg);
	_log("", msg, ap);
}

void Logging::log_debug(std::string msg, ...) {
	if(!Logging::debug) return;

	va_list ap;
	va_start(ap, msg);
	_log("DEBUG: ", msg, ap);
}

void Logging::log_info(std::string msg, ...) {
	va_list ap;
	va_start(ap, msg);
	_log("INFO: ", msg, ap);
}

void Logging::log_warning(std::string msg, ...) {
	va_list ap;
	va_start(ap, msg);
	_log("WARNING: ", msg, ap);
}

void Logging::log_critical(std::string msg, ...) {
	va_list ap;
	va_start(ap, msg);
	_log("CRITICAL: ", msg, ap);
}

void Logging::_log(std::string descr, std::string msg, va_list &ap) {
	vfprintf(stderr, (descr+msg).c_str(), ap);
	va_end(ap);
}
