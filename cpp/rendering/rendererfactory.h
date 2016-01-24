#ifndef MAYA_RENDERER_FACTORY
#define MAYA_RENDERER_FACTORY

#include <memory>

class Renderer;

namespace MayaTo
{
	class MayaRendererFactory
	{
	public:
		void createRenderer();
		sharedPtr<Renderer> getRenderer();
		void deleteRenderer();
	};
};

#endif