#include <maya/MDagPath.h>
#include <maya/MItDag.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnCamera.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnInstancer.h>
#include <maya/MFnParticleSystem.h>
#include <maya/MMatrixArray.h>
#include <maya/MIntArray.h>
#include <maya/MLightLinks.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/M3dView.h>
#include <maya/MRenderView.h>
#include <maya/MVectorArray.h>
#include <maya/MFileIO.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MFnComponent.h>

#include "mayascene.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrtools.h"
#include "utilities/pystring.h"
#include "shadingtools/shadingutils.h"
#include "threads/renderqueueworker.h"
#include "rendering/renderer.h"
#include "world.h"
#include "definitions.h"

static Logging logger;

MayaScene::MayaScene()
{
	Logging::debug("MayaScene::MayaScene()");
	this->init();
}

void MayaScene::init()
{
	this->cando_ipr = false;
	this->good = true;
	this->renderType = NORMAL;
	this->renderState = MayaScene::UNDEF;
	this->renderingStarted = false;
}

MayaScene::~MayaScene()
{
	Logging::debug("MayaScene::~MayaScene()");
}

// Here is checked if the linklist is complete, what means that we do not have to do any 
// complicated light linking, but default procedures. If excludedLights is > 0 then we found 
// any excluded Lights.
bool MayaScene::listContainsAllLights(MDagPathArray& linkedLights, MDagPathArray& excludedLights)
{
	excludedLights.clear();	
	for( uint lId = 0; lId < this->lightList.size(); lId++)
	{
		MDagPath path = this->lightList[lId]->dagPath;
		bool found = false;
		for( uint liId = 0; liId < linkedLights.length(); liId++)
		{
			MDagPath linkedPath = linkedLights[liId];
			if( linkedPath == path )
			{
				found = true;
				break;
			}
		}		
		if( found )
			continue;
		else
			excludedLights.append(path);
	}
	
	if( excludedLights.length() > 0)
		return false;

	return true;
}

bool MayaScene::lightObjectIsInLinkedLightList(sharedPtr<MayaObject> lightObject, MDagPathArray& linkedLightsArray)
{
	for (uint lId = 0; lId < linkedLightsArray.length(); lId++)
	{
		if (lightObject->mobject == linkedLightsArray[lId].node())
			return true;
	}
	return false;
}

// we have to take care for the component assignments in light linking. 
// if a mesh has per face shader assignments, we have to ask for the components to get the correct light linking
// because lightLink.getLinkedLights() will give us a wrong result in this case.

void MayaScene::getLightLinking()
{
	//Logging::debug(MString("----------- MayaScene::getLightLinking ---------------"));
	MLightLinks lightLink;
    bool parseStatus;
	parseStatus = lightLink.parseLinks(MObject::kNullObj);

	std::vector<sharedPtr<MayaObject>>::iterator oIt;
	for (oIt = objectList.begin(); oIt != objectList.end(); oIt++)
	{
		sharedPtr<MayaObject> obj = *oIt;
		MDagPathArray lightArray;

		if (!obj->mobject.hasFn(MFn::kMesh) && !obj->mobject.hasFn(MFn::kNurbsSurface) && !obj->mobject.hasFn(MFn::kNurbsCurve))
			continue;

		if (obj->mobject.hasFn(MFn::kMesh))
		{
			MObjectArray shadingGroups, components;
			MFnMesh meshFn(obj->mobject);
			meshFn.getConnectedSetsAndMembers(obj->instanceNumber, shadingGroups, components, true);	
			//Logging::debug(MString("Object ") + obj->shortName + " has " + components.length() + " component groups and " + shadingGroups.length() + " shading groups.");
			int componentElements = 0;
			for (uint cId = 0; cId < components.length(); cId++)
			{
				MFnComponent compFn(components[cId]);
				if (compFn.componentType() == MFn::kMeshPolygonComponent)
					componentElements += compFn.elementCount();
			}
			if ((shadingGroups.length() > 1) || (componentElements > 0))
			{
				Logging::debug(MString("Object ") + obj->shortName + " has " + components.length() + " component groups and " + shadingGroups.length() + " shading groups.");
				for (uint cId = 0; cId < components.length(); cId++)
				{
					MDagPathArray tmpLightArray;
					lightLink.getLinkedLights(obj->dagPath, components[cId], tmpLightArray); // Lights linked to the face component
					for (uint lp = 0; lp < tmpLightArray.length(); lp++)
						lightArray.append(tmpLightArray[lp]);
				}
			}
			else{
				lightLink.getLinkedLights(obj->dagPath, MObject::kNullObj, lightArray);
			}
		}
		else{
			lightLink.getLinkedLights(obj->dagPath, MObject::kNullObj, lightArray);
		}
		// if one of the light in my scene light list is NOT in the linked light list,
		// the light has either turned off "Illuminate by default" or it is explicilty not linked to this object.
		for (size_t lObjId = 0; lObjId < this->lightList.size(); lObjId++)
		{
			if (lightObjectIsInLinkedLightList(this->lightList[lObjId], lightArray))
			{
				//Logging::debug(MString("Light object ") + this->lightList[lObjId]->shortName + " IS in lightList -> linked.");
			}
			else{
				//Logging::debug(MString("Light object ") + this->lightList[lObjId]->shortName + " is NOT in lightList -> " + obj->shortName + " is not linked.");
				this->lightList[lObjId]->excludedObjects.push_back(obj);
			}
		}
	}
}

void MayaScene::getPasses()
{}


