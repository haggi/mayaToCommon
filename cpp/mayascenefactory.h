#ifndef MAYA_SCENE_FACTORY
#define MAYA_SCENE_FACTORY

#include "definitions.h"

class MayaScene;

namespace MayaTo
{
	class MayaSceneFactory
	{
	public:
		void createMayaScene();
		sharedPtr<MayaScene> getMayaScene();
		void deleteMayaScene();
	};
}

#endif
