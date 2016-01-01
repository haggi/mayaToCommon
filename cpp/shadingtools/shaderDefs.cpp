/*
*/

#include "shadingNode.h"
#include "shaderDefs.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/pystring.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional/optional.hpp>

using boost::property_tree::ptree;

std::vector<ShadingNode> ShaderDefinitions::shadingNodes;
bool ShaderDefinitions::readDone;

ShaderDefinitions::ShaderDefinitions()
{
	ShaderDefinitions::readDone = false;
}

ShaderDefinitions::~ShaderDefinitions()
{}

void ShaderDefinitions::readShaderDefinitions()
{

	ptree pt;
	std::string shaderDefFile = (getRendererHome() + "resources/shaderDefinitions.xml").asChar();
	std::ifstream shaderFile(shaderDefFile.c_str());
	if (!shaderFile.good())
	{
		Logging::error("Unable to read xml shader defs.");
		shaderFile.close();
		return;
	}
	read_xml(shaderDefFile, pt);

	BOOST_FOREACH(ptree::value_type v, pt.get_child("shaders"))
	{
		Logging::debug(MString("\tShader: ") + v.second.get_child("name").data().c_str());
		ShadingNode snode;
		snode.fullName = v.second.get_child("name").data().c_str();
		snode.typeName = snode.fullName;
		BOOST_FOREACH(ptree::value_type input, v.second.get_child("inputs"))
		{
			Logging::debug(MString("\tInput: ") + input.second.get_child("name").data().c_str() + " Type: " + input.second.get_child("type").data().c_str());
			ShaderAttribute att;
			att.name = input.second.get_child("name").data();
			att.type = input.second.get_child("type").data();
			int hint = input.second.count("hint");
			if (hint > 0)
			{
				att.hint = input.second.get_child("hint").data();
				att.hint = pystring::replace(att.hint, "\"", "");
				att.hint = pystring::replace(att.hint, " ", "");
			}
			snode.addInputAttribute(att);
			//if (input.second.count("min") > 0)
			//{
			//	std::cout << "\t\tMin: " << input.second.get<float>("min") << "\n";
			//}
			//if (input.second.count("max") > 0)
			//{
			//	std::cout << "\t\tMax: " << input.second.get_child("max").data() << "\n";
			//}
			//if (input.second.count("default") > 0)
			//{
			//	std::cout << "\t\tDefault: " << input.second.get_child("default").data() << "\n";
			//}
		}
		BOOST_FOREACH(ptree::value_type input, v.second.get_child("outputs"))
		{
			Logging::debug(MString("\tOutput: ") + input.second.get_child("name").data().c_str() + " Type: " + input.second.get_child("type").data().c_str());
			ShaderAttribute att;
			att.name = input.second.get_child("name").data();
			att.type = input.second.get_child("type").data();
			snode.addOutputAttribute(att);
		}
		ShaderDefinitions::shadingNodes.push_back(snode);
	}

	readDone = true;
}

bool ShaderDefinitions::findShadingNode(MObject node, ShadingNode& snode)
{
	if (!ShaderDefinitions::readDone)
		ShaderDefinitions::readShaderDefinitions();

	MString nodeTypeName = getDepNodeTypeName(node);
	for (auto sn : ShaderDefinitions::shadingNodes)
	{
		if (sn.typeName == nodeTypeName)
		{
			snode = sn;
			snode.setMObject(node);
			return true;
		}
	}
	return false;
}

bool ShaderDefinitions::findShadingNode(MString nodeTypeName, ShadingNode& snode)
{
	if (!ShaderDefinitions::readDone)
		ShaderDefinitions::readShaderDefinitions();

	for (auto sn : ShaderDefinitions::shadingNodes)
	{
		if (sn.typeName == nodeTypeName)
		{
			snode = sn;
			return true;
		}
	}
	return false;
}
