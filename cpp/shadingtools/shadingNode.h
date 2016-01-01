#ifndef MT_SHADING_TOOLS_SHADINGNODE_H
#define MT_SHADING_TOOLS_SHADINGNODE_H

#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MObjectArray.h>
#include <maya/MString.h>
#include <string>
#include <vector>

struct ShaderAttribute
{
	ShaderAttribute()
	{}
	enum AttributeType {
		NONE=0,
		MESSAGE,
		FLOAT,
		COLOR,
		VECTOR,
		STRING,
		BOOL,
		MATRIX
	};
	std::string name;
	std::string type;
	std::string hint;
	AttributeType atype;
};

#define SPLUG_LIST std::vector<ShadingPlug>

class ShadingNode
{
public:
	MString typeName; //Lambert, MultiplyDivide
	MString fullName; //myLambert1, mdivi_number_123
	MObject mobject;
	std::vector<ShaderAttribute> inputAttributes;
	std::vector<ShaderAttribute> outputAttributes;

	ShadingNode(MObject& object);
	ShadingNode(const ShadingNode &other);
	ShadingNode();
	~ShadingNode();

	bool operator==(ShadingNode const& otherOne)
	{
		return mobject == otherOne.mobject;
	}

	void setMObject(MObject object);
	bool isAttributeValid(MString attributeName);
	bool isInPlugValid(MPlug plug);
	bool isOutPlugValid(MPlug plug);
	void getConnectedInputObjects(MObjectArray& objectArray);
	void getConnectedOutputObjects(MObjectArray& objectArray);
	void addInputAttribute(ShaderAttribute att)
	{
		inputAttributes.push_back(att);
	}
	void addOutputAttribute(ShaderAttribute att)
	{
		outputAttributes.push_back(att);
	}
private:
};

#endif
