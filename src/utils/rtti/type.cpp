#include <string.h>
#include <stdio.h>
#include "utils/rtti/type.h"

RTTIType RTTIType::unknownType(RTTIType::RTTI_UNKNOWN);
RTTIType RTTIType::voidType(RTTIType::RTTI_VOID);

RTTIType RTTIType::charType(RTTIType::RTTI_CHAR);
RTTIType RTTIType::ucharType(RTTIType::RTTI_UCHAR);
RTTIType RTTIType::scharType(RTTIType::RTTI_SCHAR);

RTTIType RTTIType::shortType(RTTIType::RTTI_SHORT);
RTTIType RTTIType::ushortType(RTTIType::RTTI_USHORT);
RTTIType RTTIType::intType(RTTIType::RTTI_INT);
RTTIType RTTIType::uintType(RTTIType::RTTI_UINT);
RTTIType RTTIType::longType(RTTIType::RTTI_LONG);
RTTIType RTTIType::ulongType(RTTIType::RTTI_ULONG);
RTTIType RTTIType::i64Type(RTTIType::RTTI_I64);
RTTIType RTTIType::ui64Type(RTTIType::RTTI_UI64);
RTTIType RTTIType::floatType(RTTIType::RTTI_FLOAT);
RTTIType RTTIType::doubleType(RTTIType::RTTI_DOUBLE);
RTTIType RTTIType::boolType(RTTIType::RTTI_BOOL);


RTTIType::~RTTIType() {}

char* RTTIType::getTypeName(char* buf)
{
	char p[20] ="???";

	switch(tag)
	{
	case RTTI_UNKNOWN:
	    strcpy(p,"???");
		break;
	case RTTI_VOID:
		strcpy(p,"void");
		break;
	case RTTI_BYTE:
		strcpy(p, "byte");
		break;
	case RTTI_CHAR:
		strcpy(p,"char");
		break;
	case RTTI_UCHAR:
		strcpy(p,"unsigned char");
		break;
	case RTTI_SCHAR:
		strcpy(p,"signed char");
		break;
	case RTTI_WCHAR:
		strcpy(p,"w_char");
		break;
	case RTTI_SHORT:
		strcpy(p,"short");
		break;
	case RTTI_USHORT:
		strcpy(p,"unsigned short");
		break;
	case RTTI_INT:
		strcpy(p,"int");
		break;
	case RTTI_UINT:
		strcpy(p,"unsigned int");
		break;
	case RTTI_LONG:
		strcpy(p,"long");
		break;
	case RTTI_ULONG:
		strcpy(p,"uint64_t");
		break;
	case RTTI_I64:
		strcpy(p,"int64_t");
		break;
	case RTTI_UI64:
		strcpy(p,"unsigned int64_t");
		break;
	case RTTI_ENUM:
		strcpy(p,"enum");
		break;
	case RTTI_FLOAT:
		strcpy(p,"float");
		break;
	case RTTI_DOUBLE:
		strcpy(p,"double");
		break;
	case RTTI_BOOL:
		strcpy(p,"bool");
		break;
	}

	strcpy(buf, p);
	return buf;
}


char* RTTIPtrType::getTypeName(char* buf)
{
	char* oldbuf = buf;
	ptrType->getTypeName(buf);
	buf += strlen(buf);
	*buf++ = '*';
	*buf = '\0';
	return oldbuf;
}

char* RTTIArrayType::getTypeName(char* buf)
{
	char* oldbuf = buf;
	elemType->getTypeName(buf);
	buf += strlen(buf);
	*buf++ = '[';

	if(nElems != 0)
	{
		buf += sprintf(buf, "%d", nElems);
	}

	*buf++ = ']';
	*buf = '\0';
	return oldbuf;
}


char* RTTIDerivedType::getTypeName(char* buf)
{
	baseClass->getTypeName(buf);
	return buf;
}

char* RTTIMethodType::getTypeName(char* buf)
{
	char* oldbuf = buf;
	returnType->getTypeName(buf);
	buf += strlen(buf);
	*buf++  = '(';
	methodClass->getTypeName(buf);
	buf += strlen(buf);
	*buf++  = ':';
	*buf++  = ':';
	*buf++  = '*';
	*buf++  = ')';
	*buf++  = '(';

	for(int i = 0; i < nParams; i++)
	{
		if(i != 0)
		{
			*buf++ = ',';
		}

		paramTypes[i]->getTypeName(buf);
		buf += strlen(buf);
	}

	*buf++ = ')';
	*buf = '\0';
	return oldbuf;
}


char* RTTIMethodType::getMethodDeclaration(char* buf, char const* name)
{
	char* oldbuf = buf;

	if(methodobj && methodobj->isStatic())
	{
		buf += sprintf(buf, "%s	", "static");
		buf += strlen(buf);
	}
	else if(methodobj && methodobj->isVirtual())
	{
		buf += sprintf(buf, "%s	", "virtual");
		buf += strlen(buf);
	}

	returnType->getTypeName(buf);
	buf += strlen(buf);
	*buf++ = ' ';
	*buf++ = '\t';
	methodClass->getTypeName(buf);
	buf += strlen(buf);
	buf += sprintf(buf, "::%s(", name);

	for(int i = 0; i < nParams; i++)
	{
		if(i != 0)
		{
			*buf++ = ',';
		}

		paramTypes[i]->getTypeName(buf);
		buf += strlen(buf);
	}

	*buf++ = ')';
	*buf++ = ';';
	buf += strlen(buf);

	if(methodobj && methodobj->isAbstract())
	{
		buf += sprintf(buf, "	%s", "abstract");
		buf += strlen(buf);
	}
	else if(methodobj && methodobj->isOverloaded())
	{
		buf += sprintf(buf, "	%s", "overloaded; ");
		buf += strlen(buf);
	}

	*buf = '\0';
	return oldbuf;
}



