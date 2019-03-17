/*------------- zXMLParser.cpp
*
* Copyright (C): www.7cool.cn (2010)
* Author       :
* Version      : V1.01
* Date         : 2010/11/26 9:37:03
*
*/
/*************************************************************
*XML
*************************************************************/
#include <string.h>
#include <iostream>
#include "serialize/xmlParse.h"

/*************************************************************/
zXMLParser::zXMLParser()
{
	doc.Clear();
}


zXMLParser::~zXMLParser()
{
	doc.Clear();
}

bool zXMLParser::initFile(const std::string &xmlFile)
{
	doc.Clear();
	return doc.LoadFile(xmlFile.c_str());
}


bool zXMLParser::initFile(const char *xmlFile)
{
	doc.Clear();
	return doc.LoadFile(xmlFile);
}

bool zXMLParser::initStr(const std::string &xmlStr)
{
	doc.Clear();
	return doc.Parse(xmlStr.c_str()) != NULL;
}


bool zXMLParser::initStr(const char *xmlStr)
{
	doc.Clear();
	return doc.Parse(xmlStr) != NULL;
}

bool zXMLParser::init()
{
	doc.Clear();
	return (doc.InsertEndChild(TiXmlDeclaration("1.0", "GB2312", "")) != NULL);
}

void zXMLParser::final()
{
	doc.Clear();
}


bool zXMLParser::dump(const char* filename)
{
	if(!filename)
	{
		return false;
	}

	remove(filename);

	if(doc.RootElement() != NULL)
	{
		return doc.SaveFile(filename);
	}

	return false;
}

bool zXMLParser::dump(xmlNodePtr dumpNode, const char* filename)
{
	if(!dumpNode || !filename)
	{
		return false;
	}

	remove(filename);
	TiXmlDocument doc;
	xmlNodePtr RootElement = doc.InsertEndChild(TiXmlDeclaration("1.0", "GB2312", ""));

	if(RootElement != NULL)
	{
		doc.LinkEndChild(RootElement);
		return doc.SaveFile(filename);
	}

	return false;
}

xmlNodePtr zXMLParser::getRootNode(const char *rootName)
{
	TiXmlElement *RootElement = doc.RootElement();

	while(RootElement != NULL && rootName != NULL)
	{
		if(stricmp_q(rootName, RootElement->Value()) == 0)
		{
			return RootElement;
		}

		RootElement = RootElement->NextSiblingElement();
	}

	return RootElement;
}


xmlNodePtr zXMLParser::getChildNode(const xmlNodePtr parent, const char *childName)
{
	if(!parent)
	{
		return NULL;
	}

	if(childName)
	{
		return (TiXmlElement *)parent->FirstChildElement(childName);
	}
	else
	{
		return (TiXmlElement *)parent->FirstChildElement();
	}
}

xmlNodePtr zXMLParser::getNextNode(const xmlNodePtr node, const char *nextName)
{
	if(!node)
	{
		return NULL;
	}

	if(nextName)
	{
		return (TiXmlElement *)node->NextSiblingElement(nextName);
	}
	else
	{
		return (TiXmlElement *)node->NextSiblingElement();
	}
}

unsigned int zXMLParser::getChildNodeCount(const xmlNodePtr parent, const char *childName)
{
	if(!parent)
	{
		return 0;
	}

	unsigned int nret = 0;
	TiXmlElement *Element = NULL;

	if(childName)
	{
		Element = (TiXmlElement *)parent->FirstChildElement(childName);
	}
	else
	{
		Element = (TiXmlElement *)parent->FirstChildElement();
	}

	while(Element)
	{
		nret++;

		if(childName)
		{
			Element = Element->NextSiblingElement(childName);
		}
		else
		{
			Element = Element->NextSiblingElement();
		}
	}

	return nret;
}

#include"utils/string/stringex.h"

xmlNodePtr zXMLParser::newRootNode(const char *rootName)
{
	TiXmlElement *RootElement = new TiXmlElement(rootName);
	DEFINE_ADD_ONE(RootElement);

	if(RootElement)
	{
		doc.LinkEndChild(RootElement);
	}

	return RootElement;
}

xmlNodePtr zXMLParser::newChildNode(const xmlNodePtr parent, const char *childName, const char *content)
{
	if(!parent)
	{
		return NULL;
	}

	TiXmlElement *Element = new TiXmlElement(childName);
	DEFINE_ADD_ONE(Element);

	if(Element)
	{
		if(content)
		{
			TiXmlText *xmlcontent = new TiXmlText(content);
			DEFINE_ADD_ONE(xmlcontent);
			Element->LinkEndChild(xmlcontent);
		}

		((xmlNodePtr)parent)->LinkEndChild(Element);
	}

	return Element;
}

xmlNodePtr zXMLParser::newChildNode_Num(const xmlNodePtr parent, const char *childName, int64_t content)
{
	char szvalue[128];
	ui64toa_q(content, szvalue, 10);
	return newChildNode(parent, childName, szvalue);
}

bool zXMLParser::newNodeProp(const xmlNodePtr node, const char *propName, const char *prop)
{
	if(node && node->ToElement())
	{
		((TiXmlElement *)node->ToElement())->SetAttribute(propName, prop);
		return true;
	}

	return false;
}

bool zXMLParser::newNodeProp_Num(const xmlNodePtr node, const char *propName, int64_t prop)
{
	char szvalue[128];
	ui64toa_q(prop, szvalue, 10);
	return newNodeProp(node, propName, szvalue);
}

bool zXMLParser::_getChildNodeNum(const xmlNodePtr node, const char *Name, int64_t& value)
{
	xmlNodePtr clnode = getChildNode(node, Name);

	if(clnode && clnode->ToElement())
	{
		const char *szvalue = clnode->ToElement()->GetText();

		if(szvalue)
		{
			value = strto64_q(szvalue,NULL,10);
		}

		return true;
	}

	return false;
}

bool zXMLParser::_getNodePropNum(const xmlNodePtr node, const char *propName, int64_t& value)
{
	if(node && node->ToElement())
	{
		TiXmlAttribute *IDAttribute = ((TiXmlElement *)(node->ToElement()))->FirstAttribute();

		while(IDAttribute != NULL)
		{
			if(stricmp_q(IDAttribute->Name(), propName) == 0)
			{
				break;
			}

			IDAttribute = IDAttribute->Next();
		}

		if(IDAttribute)
		{
			const char *szvalue = IDAttribute->Value();

			if(szvalue)
			{
				value = strto64_q(szvalue,NULL,10);
			}

			return true;
		}
	}

	return false;
}

bool zXMLParser::getChildNodeStr(const xmlNodePtr node, const char *Name, void *prop, int propSize)
{
	xmlNodePtr clnode = getChildNode(node, Name);

	if(clnode && clnode->ToElement())
	{
		const char *szvalue = clnode->ToElement()->GetText();

		if(szvalue)
		{
			strcpy_q((char *)prop, propSize, szvalue);
		}

		return true;
	}

	return false;
}

bool zXMLParser::getChildNodeStr(const xmlNodePtr node, const char *Name, std::string &prop)
{
	xmlNodePtr clnode = getChildNode(node, Name);

	if(clnode && clnode->ToElement())
	{
		const char *szvalue = clnode->ToElement()->GetText();
		prop = szvalue;
		return true;
	}

	return false;
}

bool zXMLParser::getNodePropStr(const xmlNodePtr node, const char *propName, void *prop, int propSize)
{
	if(node && node->ToElement())
	{
		TiXmlAttribute *IDAttribute = ((TiXmlElement *)(node->ToElement()))->FirstAttribute();

		while(IDAttribute != NULL)
		{
			if(stricmp_q(IDAttribute->Name(), propName) == 0)
			{
				break;
			}

			IDAttribute = IDAttribute->Next();
		}

		if(IDAttribute)
		{
			const char *szvalue = IDAttribute->Value();

			if(szvalue)
			{
				strcpy_q((char *)prop, propSize, szvalue);
			}

			return true;
		}
	}

	return false;
}

bool zXMLParser::getNodePropStr(const xmlNodePtr node, const char *propName, std::string &prop)
{
	if(node && node->ToElement())
	{
		TiXmlAttribute *IDAttribute = ((TiXmlElement *)(node->ToElement()))->FirstAttribute();

		while(IDAttribute != NULL)
		{
			if(stricmp_q(IDAttribute->Name(), propName) == 0)
			{
				break;
			}

			IDAttribute = IDAttribute->Next();
		}

		if(IDAttribute)
		{
			const char *szvalue = IDAttribute->Value();
			prop = szvalue;
			return true;
		}
	}

	return false;
}
