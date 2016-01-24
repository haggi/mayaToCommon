#ifndef MAYA_SCENE_H
#define MAYA_SCENE_H

#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MDagPathArray.h>
#include <maya/MTransformationMatrix.h>
#include <map>
#include <vector>

#include "renderglobals.h"
#include "mayaobject.h"
#include "definitions.h"
#include "utilities/minimap.h"

struct InteractiveElement{
	
	sharedPtr<MayaObject> obj;
	MObject mobj;
	MString name;
	MObject node;
	bool triggeredFromTransform = false; // to recognize if we have to update the shape or only the instance transform
};

class MayaScene
{
public:
	enum RenderType{
		NORMAL, 
		IPR,
		NONE,
		BATCH
	};
	
	enum RenderState{
		START = 0, 
		TRANSLATE = 1,
		RENDERING = 2,
		FRAMEDONE = 3,
		RENDERERROR = 4,
		UNDEF = 5
	};

	bool good;
	RenderType renderType;
	RenderState renderState;
	std::vector<int> lightIdentifier; // plugids for detecting new lighttypes
	std::vector<int> objectIdentifier; // plugids for detecting new objTypes
	std::vector<MObject> mObjectList;
	std::vector<sharedPtr<MayaObject>>  objectList;
	std::vector<sharedPtr<MayaObject>>  camList;
	std::vector<sharedPtr<MayaObject>>  lightList;
	std::vector<sharedPtr<MayaObject>>  instancerNodeElements; // so its easier to update them
	std::vector<MDagPath> instancerDagPathList;

	MiniMap<MObject, sharedPtr<MayaObject> > mayaObjMObjMap;


	float currentFrame;	
	bool parseSceneHierarchy(MDagPath currentObject, int level, sharedPtr<ObjectAttributes> attr, sharedPtr<MayaObject> parentObject); // new, parse whole scene as hierarchy and save/analyze objects
	bool parseScene();
	bool renderingStarted;
	bool parseInstancerNew(); // parse only particle instancer nodes, its a bit more complex

	bool cando_ipr;
	bool canDoIPR();

	MDagPath uiCamera;

	MFn::Type updateElement;
	bool updateScene(); // update all necessary objects
	bool updateScene(MFn::Type updateElement); // update all necessary objects
	bool updateInstancer(); // update all necessary objects
	MString getExportPath(MString ext, MString rendererName);
	MString getFileName();
	
	void clearInstancerNodeList();
	void clearObjList(std::vector<sharedPtr<MayaObject>> & objList);
	void clearObjList(std::vector<sharedPtr<MayaObject>> & objList, sharedPtr<MayaObject> notThisOne);
	bool lightObjectIsInLinkedLightList(sharedPtr<MayaObject> lightObject, MDagPathArray& linkedLightsArray);
	void getLightLinking();
	bool listContainsAllLights(MDagPathArray& linkedLights, MDagPathArray& excludedLights);
	MDagPath getWorld();

	void getPasses();
	void setCurrentCamera(MDagPath camera);
	void checkParent(sharedPtr<MayaObject> obj);

	void classifyMayaObject(sharedPtr<MayaObject> obj);
	bool isGeo(MObject obj);
	bool isLight(MObject obj);

	//void waitForFrameCompletion();
	void setRenderType(RenderType rtype);
	sharedPtr<MayaObject> getObject(MObject obj);
	sharedPtr<MayaObject> getObject(MDagPath dp);
	void init();
	MayaScene();
	~MayaScene();

	// interactive elements
	std::map<uint, InteractiveElement> interactiveUpdateMap;
	void updateInteraciveRenderScene(std::vector<InteractiveElement *> elementList);

};

#endif
