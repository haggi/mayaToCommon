#include "renderprocess.h"
#include <maya/MGlobal.h>
#include <maya/MRenderView.h>
#include "world.h"
#include "renderglobals.h"
#include "mayascene.h"
#include "utilities/logging.h"
#include "threads/renderqueueworker.h"

namespace RenderProcess{

	void doPreRenderJobs()
	{
	}

	// not sure if this is really the best way to do it. 
	// the renderer should be able to access all scene element lists and these are creating in parseScene() 
	// but the preFrameScripts should be called before the whole parsing is done because it is possible that this script 
	// updates or creates geometry.
	void doRenderPreFrameJobs()
	{
		MayaTo::getWorldPtr()->worldRendererPtr->preFrame();
	}
	
	void doPreFrameJobs()
	{
		MString result;
		MGlobal::executeCommand(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->preFrameScript, result, true);
	}
	void doPostFrameJobs()
	{
		MString result;
		MGlobal::executeCommand(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->postFrameScript, result, true);
		MayaTo::getWorldPtr()->worldRendererPtr->postFrame();
	}

	void doPostRenderJobs(){}
	void doPrepareFrame()
	{
		float currentFrame = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->getFrameNumber();
		sharedPtr<MayaScene> mayaScene = MayaTo::getWorldPtr()->worldScenePtr;
		Logging::progress(MString("\n========== doPrepareFrame ") + currentFrame + " ==============\n");

		mayaScene->parseScene(); // all lists are cleaned and refilled with the current scene content
		std::vector<sharedPtr<MayaObject>>::iterator oIt;
		for (oIt = mayaScene->camList.begin(); oIt != mayaScene->camList.end(); oIt++)
		{
			sharedPtr<MayaObject> camera = *oIt;
			if (!isCameraRenderable(camera->mobject) && (!(camera->dagPath == mayaScene->uiCamera)))
			{
				Logging::debug(MString("Camera ") + camera->shortName + " is not renderable, skipping.");
				continue;
			}
			Logging::info(MString("Rendering camera ") + camera->shortName);
			if (MGlobal::mayaState() != MGlobal::kBatch)
			{
				MRenderView::setCurrentCamera(camera->dagPath);
			}
		}

		MayaTo::getWorldPtr()->worldRenderGlobalsPtr->getMbSteps();

		if (MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList.size() == 0)
		{
			Logging::error(MString("no mb steps, something's wrong."));
			return;
		}

		int numMbSteps = (int)MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList.size();

		for (int mbStepId = 0; mbStepId < numMbSteps; mbStepId++)
		{
			MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentMbStep = mbStepId;
			MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentMbElement = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId];
			MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber = (float)(currentFrame + MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId].time);
			bool needView = true;

			// we can have some mb time steps at the same time, e.g. for xform and deform, then we do not need to update the view
			if (mbStepId > 0)
			{
				if (MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId].time == MayaTo::getWorldPtr()->worldRenderGlobalsPtr->mbElementList[mbStepId - 1].time)
				{
					needView = false;
				}
			}

			if (needView)
			{
				Logging::debug(MString("doFrameJobs() viewFrame: ") + MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber);
				MGlobal::viewFrame(MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentFrameNumber);
			}

			if (MayaTo::getWorldPtr()->worldScenePtr)
				mayaScene->updateScene();
			else
				Logging::error(MString("no maya scene ptr."));

			Logging::info(MString("update scene done"));
			MayaTo::getWorldPtr()->worldRenderGlobalsPtr->currentMbStep++;
		}

		if (MGlobal::mayaState() != MGlobal::kBatch)
			MGlobal::viewFrame(currentFrame);
		//}
	}

	void doFrameJobs()
	{
		Logging::debug("doFrameJobs()");
	}

	void render()
	{
	}

}
