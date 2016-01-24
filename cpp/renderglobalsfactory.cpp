#include "world.h"
#include "renderglobalsfactory.h"
#include "renderglobals.h"

namespace MayaTo
{
	void RenderGlobalsFactory::createRenderGlobals()
	{
		getWorldPtr()->worldRenderGlobalsPtr = sharedPtr<RenderGlobals> (new RenderGlobals());
	}
	sharedPtr<RenderGlobals> RenderGlobalsFactory::getRenderGlobals()
	{
		return getWorldPtr()->worldRenderGlobalsPtr;
	}
	void RenderGlobalsFactory::deleteRenderGlobals()
	{
		getWorldPtr()->worldRenderGlobalsPtr.reset();
	}
};