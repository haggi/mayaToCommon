#ifndef MAYATO_DEFS_H
#define MAYATO_DEFS_H

#ifdef NOCPP11
#else
#endif

#ifdef NOCPP11
	#include "boost/shared_ptr.hpp"
	#include "boost/smart_ptr.hpp"
	#include "boost/thread.hpp"
	#define sharedPtr boost::shared_ptr
	#define autoPtr boost::shared_ptr
	#define staticPtrCast boost::static_pointer_cast
	#define releasePtr(x) x.reset()
	#define sleepFor(x) boost::this_thread::sleep(boost::posix_time::milliseconds(x))
	#define threadObject boost::thread
#else
	#include <memory>
	#include <thread>
	#define sharedPtr std::shared_ptr
	#define autoPtr std::auto_ptr
	#define staticPtrCast std::static_pointer_cast
	#define releasePtr(x) x.release()
	#define sleepFor(x) std::this_thread::sleep(std::chrono::milliseconds(x))
	#define threadObject threadObject
#endif

#endif