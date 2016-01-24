#include "mayascenefactory.h"
#include "mayascene.h"
#include "world.h"
#include <memory>

namespace MayaTo
{
	void MayaSceneFactory::createMayaScene()
	{
		getWorldPtr()->worldScenePtr = sharedPtr<MayaScene> (new MayaScene());
	}
	sharedPtr<MayaScene> MayaSceneFactory::getMayaScene()
	{
		return getWorldPtr()->worldScenePtr;
	}
	void MayaSceneFactory::deleteMayaScene()
	{
		getWorldPtr()->worldScenePtr.reset();
	}
}

