#include <string.h>
#include <stdlib.h>
#include "utils/rtti/type.h"

RTTIFieldDescriptor* RTTIClassDescriptor::findFieldByAliasName(char const* name, int nbegin, int flag, bool bocasestr)
{
	for(int i = nbegin; i < nFields; i++)
	{
		if((fields[i]->flags & flag) == flag && (strcmp(fields[i]->aliasname, name) == 0 || (bocasestr && stricmp_q(fields[i]->aliasname, name) == 0)))
		{
			return fields[i];
		}
	}

	return NULL;
}

RTTIFieldDescriptor* RTTIClassDescriptor::findField(char const* name, int nbegin, int flag)
{
	for(int i = nbegin; i < nFields; i++)
	{
		if((fields[i]->flags & flag) == flag && strcmp(fields[i]->name, name) == 0)
		{
			return fields[i];
		}
	}

	return NULL;
}

static int cmpFields(const void* p, const void* q)
{
	return ((int)(*(RTTIFieldDescriptor**)p)->getOffset()) - ((int)(*(RTTIFieldDescriptor**)q)->getOffset());
}

static int cmpMethods(const void* p, const void* q)
{
	return strcmp((*(RTTIMethodDescriptor**)p)->getName(), (*(RTTIMethodDescriptor**)q)->getName());
}


RTTIClassDescriptor::RTTIClassDescriptor(char const* name, int size,
        RTTIDescribeFieldsFunc  describeFieldsFunc,
        RTTIDescribeMethodsFunc describeMethodsFunc,

        int flags, int sortfieldtype, char const* aliasname,
        unsigned short int userdefine)
	: RTTIType(RTTI_STRUCT, userdefine)
{
	this->name = name;
	this->aliasname = aliasname;

	if(!this->aliasname)
	{
		this->aliasname = name;
	}

	this->size = size;
	this->flags = flags;
	this->fieldList = NULL;
	this->methodList = NULL;
	this->nFields = 0;
	this->fields = NULL;
	this->nMethods = 0;
	this->methods = NULL;
	this->nBaseClasses = 0;
	this->baseClassesFields = NULL;
	this->baseClasses = NULL;
	fieldList = NULL;

	if(describeFieldsFunc)
	{
		fieldList = (*describeFieldsFunc)();
	}

	methodList = NULL;

	if(describeMethodsFunc)
	{
		methodList = (*describeMethodsFunc)();
	}

	buildClassDescriptor(sortfieldtype);
	RTTIRepository* repo = RTTIRepository::getInstance();
	repo->addClass(this);
}













RTTIClassDescriptor::RTTIClassDescriptor(char const* name, int size, int flags, int sortfieldtype, char const* aliasname, unsigned short int userdefine)
	: RTTIType(RTTI_STRUCT, userdefine)
{
	this->name = name;
	this->aliasname = aliasname;

	if(!this->aliasname)
	{
		this->aliasname = name;
	}

	this->size = size;
	this->flags = flags;
	this->fieldList = NULL;
	this->methodList = NULL;
	this->nFields = 0;
	this->fields = NULL;
	this->nMethods = 0;
	this->methods = NULL;
	this->nBaseClasses = 0;
	this->baseClassesFields = NULL;
	this->baseClasses = NULL;
	initialized = false;
}
#include "logging/logging/zLogger.h"
void* RTTIClassDescriptor::ConvertClassPtr(void* psrc, RTTIClassDescriptor* src_cls, RTTIClassDescriptor* dst_cls)
{
	int noffset = 0;

	if(src_cls && src_cls->isClass() && src_cls->IsKindOf(dst_cls, &noffset))
	{
		return ((void*)((char*)psrc + noffset));
	}
	else if(dst_cls && dst_cls->isClass() && dst_cls->IsKindOf(src_cls, &noffset))
	{
		return ((void*)((char*)psrc - noffset));
	}
	else
	{
		char szbuf[512];
		char szbuf1[512];
		g_logger.warn(" %s  %s", ((RTTIType*)src_cls)->getTypeName(szbuf), ((RTTIType*)dst_cls)->getTypeName(szbuf1));
	}

	return NULL;
}

void* RTTIClassDescriptor::ConvertClassPtr(void* p, int ibase)
{
	return ((void*)(((char*)p) + baseClassesFields[ibase]->getOffset()));
}

bool RTTIClassDescriptor::IsKindOf(RTTIClassDescriptor* kindclass, int* offset)
{
	if(kindclass != this)
	{
		for(int i = 0; i < nBaseClasses; i++)
		{
			if(baseClasses[i]->IsKindOf(kindclass, offset))
			{
				if(offset)
				{
					(*offset) = (*offset) + baseClassesFields[i]->getOffset();
				}

				return true;
			};
		}

		return false;
	}
	else if(offset)
	{
		(*offset) = (*offset) + 0;
	}

	return true;
}


void RTTIClassDescriptor::buildClassDescriptor(int sortfieldtype)
{
	int i, n, nb;
	RTTIFieldDescriptor *fd;
	RTTIMethodDescriptor* md;

	for(fd = fieldList, n = 0, nb = 0; fd != NULL; fd = fd->next)
	{
		fd->declaringClass = this;

		if(fd->type != NULL && fd->type->tag == RTTI_DERIVED)
		{
			nb++;
		}
		else
		{
			n++;
		}
	}

	nBaseClasses = nb;
	nFields = n;
	fields = NULL;
	baseClasses = NULL;

	if(n > 0 || nb > 0)
	{
		if(n > 0)
		{
			fields = new RTTIFieldDescriptor*[n];
			DEFINE_ADD_ONE(fields);
		}

		if(nb > 0)
		{
			char* tmp = new char[(sizeof(RTTIClassDescriptor*)+sizeof(int))*(nb+2)];
			DEFINE_ADD_ONE(tmp);
			baseClasses = (RTTIClassDescriptor**)tmp;
			baseClassesFields = (RTTIFieldDescriptor**)&tmp[sizeof(RTTIClassDescriptor*)*(nb+1)];
		}

		int nf = 0;
		int nfb = 0;

		for(fd = fieldList; fd != NULL; fd = fd->next)
		{
			if(fd->type != NULL && fd->type->tag == RTTI_DERIVED && baseClasses)
			{
				if(nfb < nb)
				{
					baseClasses[nfb] = ((RTTIDerivedType*)fd->type)->getBaseClass();
					baseClassesFields[nfb] = fd;
					nfb++;
				}
			}
			else
			{
				if(nf < n)
				{
					fields[nf] = fd;
					nf++;
				}
			}
		}

		switch(sortfieldtype)
		{
		case eSortByOffSet:
			{
				qsort(fields, n, sizeof(RTTIFieldDescriptor*), cmpFields);
			}
			break;
		case eSortByName:
		default:
			{
			}
			break;
		}

		for(i = 0; i < nb; i++)
		{
			baseClassesFields[i]->index = i;
		}

		for(i = 0; i < n; i++)
		{
			fields[i]->index = i;
		}
	}

	for(n = 0, md = methodList; md != NULL; md = md->next)
	{
		n += 1;
	}

	nMethods = n;
	methods = NULL;

	if(n > 0)
	{
		methods = new RTTIMethodDescriptor*[n];
		DEFINE_ADD_ONE(methods);

		for(n = 0, md = methodList; md != NULL; md = md->next)
		{
			methods[n++] = md;
		}

		qsort(methods, n, sizeof(RTTIMethodDescriptor*), cmpMethods);

		for(i = 0; i < n; i++)
		{
			methods[i]->index = i;
		}
	}

	initialized = true;
}

char* RTTIClassDescriptor::getTypeName(char* buf)
{
	strcpy(buf, name);
	return buf;
}

RTTIMethodDescriptor* RTTIClassDescriptor::findMethodByAliasName(char const* name, int nbegin, int flag, bool bocasestr)
{
	for(int i = nbegin; i < nMethods; i++)
	{
		if((methods[i]->flags & flag) == flag && (strcmp(methods[i]->aliasname, name) == 0 || (bocasestr && stricmp_q(methods[i]->aliasname, name) == 0)))
		{
			return methods[i];
		}
	}

	return NULL;
}

RTTIMethodDescriptor* RTTIClassDescriptor::findMethod(char const* name, int nbegin, int flag)
{
	for(int i = nbegin; i < nMethods; i++)
	{
		if((methods[i]->flags & flag) == flag && strcmp(methods[i]->name, name) == 0)
		{
			return methods[i];
		}
	}

	return NULL;
}



RTTIClassDescriptor::~RTTIClassDescriptor()
{
	if(baseClassesFields)
	{
		for(int i = 0; i < nBaseClasses; i++)
		{
		    DEFINE_DEL_ONE(baseClassesFields[i]);
			SAFE_DELETE(baseClassesFields[i]);

		}
        DEFINE_DEL_ONE(baseClasses);
		SAFE_DELETE_VEC(baseClasses);

	}

	if(nMethods)
	{
		for(int i = 0; i < nMethods; i++)
		{
		    DEFINE_DEL_ONE(methods[i]);
			SAFE_DELETE(methods[i]);

		}
        DEFINE_DEL_ONE(methods);
		SAFE_DELETE_VEC(methods);

	}

	if(fields)
	{
		for(int i = 0; i < nFields; i++)
		{
		    DEFINE_DEL_ONE(fields[i]);
			SAFE_DELETE(fields[i]);

		}
        DEFINE_DEL_ONE(fields);
		SAFE_DELETE_VEC(fields);

	}
}

