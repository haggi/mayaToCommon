#ifndef MAYA_OBJECT_FACTORY
#define MAYA_OBJECT_FACTORY

#include <boost/shared_ptr.hpp>
#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include "definitions.h"

class MayaObject;

namespace MayaTo
{
	class MayaObjectFactory
	{
	public:
		sharedPtr<MayaObject> createMayaObject(MObject& mobject);
		sharedPtr<MayaObject> createMayaObject(MDagPath& objPath);
	};
}

#endif
