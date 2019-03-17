/*------------- zXMLParser.h
*
* Copyright (C): www.7cool.cn (2010)
* Author       :
* Version      : V1.01
* Date         : 2010/11/26 9:40:24
*
*/
/*************************************************************
*XML
*************************************************************/
#ifndef _XML_PARSE_H__
#define _XML_PARSE_H__


#include "tinyxml.h"
#include <string>
#include "define/define.h"
#include "platform/platform.h"
/*************************************************************/
#define xmlNodePtr TiXmlNode*


class zXMLParser
{
public:
	zXMLParser();
	~zXMLParser();

	bool initFile(const std::string &xmlFile);
	bool initFile(const char *xmlFile);
	bool initStr(const std::string &xmlStr);
	bool initStr(const char *xmlStr);
	bool init();
	void final();

	bool dump(const char* filename);
	bool dump(xmlNodePtr dumpNode, const char* filename);

	xmlNodePtr getRootNode(const char *rootName);
	xmlNodePtr getChildNode(const xmlNodePtr parent, const char *childName);
	xmlNodePtr getNextNode(const xmlNodePtr node, const char *nextName);
	unsigned int getChildNodeCount(const xmlNodePtr parent, const char *childName);

	xmlNodePtr newRootNode(const char *rootName);

	xmlNodePtr newChildNode(const xmlNodePtr parent, const char *childName, const char *content);
	xmlNodePtr newChildNode_Num(const xmlNodePtr parent, const char *childName, int64_t content);//__int64

	bool getChildNodeStr(const xmlNodePtr node, const char *Name, void *prop, int propSize);
	bool getChildNodeStr(const xmlNodePtr node, const char *Name, std::string &prop);

	bool newNodeProp(const xmlNodePtr node, const char *propName, const char *prop);
	bool newNodeProp_Num(const xmlNodePtr node, const char *propName, int64_t prop);

	bool getNodePropStr(const xmlNodePtr node, const char *propName, void *prop, int propSize);
	bool getNodePropStr(const xmlNodePtr node, const char *propName, std::string &prop);

	TiXmlDocument& getParser()
	{
		return doc;
	};
protected:
	bool _getNodePropNum(const xmlNodePtr node, const char *propName, int64_t& value);
	bool _getChildNodeNum(const xmlNodePtr node, const char *Name, int64_t& value);
public:
	template<class _Ty>	bool getNodePropNum(const xmlNodePtr node, const char *propName, _Ty &prop)
	{
	    int64_t i64value = 0;
		bool boret = _getNodePropNum(node, propName, i64value);

		if(boret)
		{
			prop = (_Ty)i64value;
		}

		return boret;
	};
	template<class _Ty>	bool getChildNodeNum(const xmlNodePtr node, const char *Name, _Ty &prop)
	{
	    int64_t i64value = 0;
		bool boret = _getChildNodeNum(node, Name, i64value);

		if(boret)
		{
			prop = (_Ty)i64value;
		}

		return boret;
	};
private:
	TiXmlDocument doc;
};

#endif //_XML_PARSE_H__
