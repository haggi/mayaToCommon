#ifndef MAYATO_DEFS_H
#define MAYATO_DEFS_H

#ifdef NOCPP11
#else
#endif

#ifdef NOCPP11
	#include "boost/shared_ptr.hpp"
	#include "boost/smart_ptr.hpp"
	#define sharedPtr boost::shared_ptr
	#define autoPtr boost::shared_ptr
	#define staticPtrCast boost::static_pointer_cast
	#define releasePtr(x) x.reset()
#else
	#include <memory>
	#define sharedPtr std::shared_ptr
	#define autoPtr std::auto_ptr
	#define staticPtrCast std::static_pointer_cast
	#define releasePtr(x) x.release()
#endif

#endif