#ifndef RENDERPROCESS_H
#define RENDERPROCESS_H

namespace RenderProcess{
	void doPreRenderJobs();
	void doPreFrameJobs();
	void doRenderPreFrameJobs();
	void doPostFrameJobs();
	void doPostRenderJobs();
	void doFrameJobs();
	void doPrepareFrame();
	void render();
}


#endif