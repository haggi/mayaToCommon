#ifndef MT_SHADING_TOOLS_SHADERDEFS_H
#define MT_SHADING_TOOLS_SHADERDEFS_H

#include <fstream>
#include <string>
#include <vector>
//#include "shadingNode.h"

class ShadingNode;

class ShaderDefinitions
{
public:
	static std::vector<ShadingNode> shadingNodes;
	static bool readDone;

	ShaderDefinitions();
	~ShaderDefinitions();
	static void readShaderDefinitions();
	static bool findShadingNode(MObject mobject, ShadingNode& snode);
	static bool findShadingNode(MString typeName, ShadingNode& snode);
	static bool shadingNodeSupported(MString typeName);
};
#endif
