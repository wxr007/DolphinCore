/*------------- type.h
*
* Copyright (C): www.7cool.cn (2010)
* Author       :
* Version      : V1.01
* Date         : 2010/11/26 9:35:39
*
*/
/*************************************************************
*
*************************************************************/
#ifndef _TYPE_H__
#define _TYPE_H__


#include <string.h>
#include"utils/string/stringex.h"
#include <stdint.h>
/*************************************************************/

class RTTIClassDescriptor;
class RTTIFieldDescriptor;
class RTTIMethodDescriptor;

enum RTTIClassFlags
{
	RTTI_CLS_ABSTRACT  = 0x0001,
	RTTI_CLS_INTERNAL  = 0x0002,
	RTTI_CLS_TRANSIENT = 0x0004
};

enum eSortFieldType
{
	eSortNone = 0,
	eSortByOffSet = 1,
	eSortByName = 2,
};



#define RTTI_MAX_PARAMETERS  5


enum RTTIFieldFlags
{
	RTTI_FLD_INSTANCE  = 0x0001,
	RTTI_FLD_STATIC    = 0x0002,
	RTTI_FLD_CONST     = 0x0004,
	RTTI_FLD_PUBLIC    = 0x0010,
	RTTI_FLD_PROTECTED = 0x0020,
	RTTI_FLD_PRIVATE   = 0x0040,
	RTTI_FLD_VIRTUAL   = 0x0100,
	RTTI_FLD_VOLATILE  = 0x0200,
	RTTI_FLD_TRANSIENT = 0x0400
};


enum RTTIMethodFlags
{
	RTTI_MTH_INSTANCE  = RTTI_FLD_INSTANCE,
	RTTI_MTH_STATIC    = RTTI_FLD_STATIC,
	RTTI_MTH_CONST     = RTTI_FLD_CONST,
	RTTI_MTH_PUBLIC    = RTTI_FLD_PUBLIC,
	RTTI_MTH_PROTECTED = RTTI_FLD_PROTECTED,
	RTTI_MTH_PRIVATE   = RTTI_FLD_PRIVATE,
	RTTI_MTH_VIRTUAL   = RTTI_FLD_VIRTUAL,
	RTTI_MTH_CONSTRUCTOR = 0x0200,
	RTTI_MTH_ABSTRACT    = 0x0400,
	RTTI_MTH_OVERLOADED  = 0x0800,
};

class  RTTIType
{
public:
	int  getTag()
	{
		return tag;
	}

	virtual char* getTypeName(char* buf);

	static RTTIType voidType;

	static RTTIType charType;
	static RTTIType ucharType;
	static RTTIType scharType;

	static RTTIType shortType;
	static RTTIType ushortType;
	static RTTIType intType;
	static RTTIType uintType;
	static RTTIType longType;
	static RTTIType ulongType;
	static RTTIType i64Type;
	static RTTIType ui64Type;
	static RTTIType floatType;
	static RTTIType doubleType;
	static RTTIType boolType;
	static RTTIType unknownType;

	enum TypeTag
	{
		RTTI_UNKNOWN,
		RTTI_VOID,

		RTTI_BYTE,
		RTTI_CHAR,
		RTTI_UCHAR,
		RTTI_SCHAR,
		RTTI_WCHAR,
		RTTI_SHORT,
		RTTI_USHORT,
		RTTI_INT,
		RTTI_UINT,
		RTTI_LONG,
		RTTI_ULONG,
		RTTI_I64,
		RTTI_UI64,
		RTTI_ENUM,

		RTTI_BOOL,

		RTTI_FLOAT,
		RTTI_DOUBLE,

		RTTI_ARRAY,
		RTTI_STRUCT,
		RTTI_PTR,
		RTTI_DERIVED,
		RTTI_METHOD
	};

	bool isBuiltin()
	{
		return (tag <= RTTI_DOUBLE) && (tag != RTTI_ENUM);
	}

	bool isScalar()
	{
		return tag > RTTI_VOID && tag <= RTTI_DOUBLE;
	}
	bool isInt()
	{
		return tag > RTTI_VOID && tag <= RTTI_BOOL;
	}
	bool isDouble()
	{
		return tag >= RTTI_FLOAT && tag <= RTTI_DOUBLE;
	}

	bool isArray()
	{
		return tag == RTTI_ARRAY;
	}

	bool isPointer()
	{
		return tag == RTTI_PTR;
	}

	virtual int getPtrLevel()
	{
		return m_nPtrLevel;
	}

	virtual RTTIType* getPtrDataType(int nBackLevel = 0x7fff)
	{
		return this;
	}

	bool isClass()
	{
		return tag == RTTI_STRUCT;
	}

	bool isBaseClass()
	{
		return tag == RTTI_DERIVED;
	}

	virtual ~RTTIType();

protected:
	friend class RTTIClassDescriptor;
	friend class RTTIFieldDescriptor;
	friend class RTTIMethodDescriptor;
	friend class RTTIPtrType;
	friend class RTTIMethodType;
	friend class RTTIArrayType;

	unsigned short int   tag;
	unsigned short int   m_nPtrLevel;
	void destroy()
	{
		if(!isBuiltin() && tag != RTTI_STRUCT)
		{
			delete this;
		}
	}

	RTTIType(unsigned short int tag)
	{
		this->tag = tag;
		m_nPtrLevel = 0;
	}

	RTTIType(unsigned short int tag, unsigned short int usersettag)
	{
		this->tag = usersettag;
		m_nPtrLevel = 0;
	}
};

class  RTTIClassDescriptor : public RTTIType
{
public:
	typedef RTTIFieldDescriptor*(*RTTIDescribeFieldsFunc)();
	typedef RTTIMethodDescriptor*(*RTTIDescribeMethodsFunc)();

	char* getTypeName(char* buf);


	RTTIClassDescriptor* getNextClass()
	{
		RTTIClassDescriptor* retnext = next;

		while(retnext)
		{
			if(retnext->isClass())
			{
				return retnext;
			}

			retnext = retnext->next;
		}

		return retnext;
	}

	RTTIClassDescriptor* getNext()
	{
		return next;
	}


	RTTIFieldDescriptor** getFields()
	{
		return fields;
	}


	int getNumberOfFields()
	{
		return nFields;
	}


	char const* getName()
	{
		return name;
	}
	char const* getAliasName()
	{
		return aliasname;
	}

	int  getSize()
	{
		return size;
	}


	int  getFlags()
	{
		return flags;
	}





//	template<class __RT>
//	bool newInstance(__RT*& retobj)
//	{
//		RTTIMethodDescriptor* pm = findMethodfunc< stInvokeCallHelper<int, __RT>::new0 >("new0", 0, RTTI_MTH_CONSTRUCTOR);
//
//		if(pm)
//		{
//			bool boret = false;
//
//			if(pm->invoke(boret, (stInvokeCallHelper<int, __RT>*)NULL, (void**)&retobj))
//			{
//				__RT* tmpobj = NULL;
//				tmpobj = (__RT*)ConvertClassPtr((void*)retobj, this, __RTTITypeOfPtr((__RT*)NULL));
//
//				if(tmpobj == NULL)
//				{
//					tmpobj = retobj;
//				}
//
//				retobj = tmpobj;
//				return boret;
//			}
//		}
//		else
//		{
//			char szbuff[512];
//			g_logger.debug("找不到与函数原型 %s 匹配的构造函数!", RTTIFuncTypeGetDis((stInvokeCallHelper<int, __RT>::new0)NULL, szbuff, this, "new0"));
//		}
//
//		return false;
//	}
//	template<class __RT, class __P1>
//	bool newInstance(__RT*& retobj, __P1 p1)
//	{
//		RTTIMethodDescriptor* pm = findMethodfunc< stInvokeCallHelper<int, __RT, __P1>::new1 >("new1", 0, RTTI_MTH_CONSTRUCTOR);
//
//		if(pm)
//		{
//			bool boret = false;
//
//			if(pm->invoke(boret, (stInvokeCallHelper<int, __RT, __P1>*)NULL, (void**)&retobj, p1))
//			{
//				__RT* tmpobj = NULL;
//				tmpobj = (__RT*)ConvertClassPtr((void*)retobj, this, __RTTITypeOfPtr((__RT*)NULL));
//
//				if(tmpobj == NULL)
//				{
//					tmpobj = retobj;
//				}
//
//				retobj = tmpobj;
//				return boret;
//			}
//		}
//		else
//		{
//			char szbuff[512];
//			g_logger.debug("找不到与函数原型 %s 匹配的构造函数!", RTTIFuncTypeGetDis((stInvokeCallHelper<int, __RT, __P1>::new1)NULL, szbuff, this, "new1"));
//		}
//
//		return false;
//	}
//	template<class __RT, class __P1, class __P2>
//	bool newInstance(__RT*& retobj, __P1 p1, __P2 p2)
//	{
//		RTTIMethodDescriptor* pm = findMethodfunc< stInvokeCallHelper<int, __RT, __P1, __P2>::new2 >("new2", 0, RTTI_MTH_CONSTRUCTOR);
//
//		if(pm)
//		{
//			bool boret = false;
//
//			if(pm->invoke(boret, (stInvokeCallHelper<int, __RT, __P1, __P2>*)NULL, (void**)&retobj, p1, p2))
//			{
//				__RT* tmpobj = NULL;
//				tmpobj = (__RT*)ConvertClassPtr((void*)retobj, this, __RTTITypeOfPtr((__RT*)NULL));
//
//				if(tmpobj == NULL)
//				{
//					tmpobj = retobj;
//				}
//
//				retobj = tmpobj;
//				return boret;
//			}
//		}
//		else
//		{
//			char szbuff[512];
//			g_logger.debug("找不到与函数原型 %s 匹配的构造函数!", RTTIFuncTypeGetDis((stInvokeCallHelper<int, __RT, __P1, __P2>::new2)NULL, szbuff, this, "new2"));
//		}
//
//		return false;
//	}
//	template<class __RT, class __P1, class __P2, class __P3>
//	bool newInstance(__RT*& retobj, __P1 p1, __P2 p2, __P3 p3)
//	{
//		RTTIMethodDescriptor* pm = findMethodfunc< stInvokeCallHelper<int, __RT, __P1, __P2, __P3>::new3 >("new3", 0, RTTI_MTH_CONSTRUCTOR);
//
//		if(pm)
//		{
//			bool boret = false;
//
//			if(pm->invoke(boret, (stInvokeCallHelper<int, __RT, __P1, __P2, __P3>*)NULL, (void**)&retobj, p1, p2, p3))
//			{
//				__RT* tmpobj = NULL;
//				tmpobj = (__RT*)ConvertClassPtr((void*)retobj, this, __RTTITypeOfPtr((__RT*)NULL));
//
//				if(tmpobj == NULL)
//				{
//					tmpobj = retobj;
//				}
//
//				retobj = tmpobj;
//				return boret;
//			}
//		}
//		else
//		{
//			char szbuff[512];
//			g_logger.debug("找不到与函数原型 %s 匹配的构造函数!", RTTIFuncTypeGetDis((stInvokeCallHelper<int, __RT, __P1, __P2, __P3>::new3)NULL, szbuff, this, "new3"));
//		}
//
//		return false;
//	}
//	template<class __RT, class __P1, class __P2, class __P3, class __P4>
//	bool newInstance(__RT*& retobj, __P1 p1, __P2 p2, __P3 p3, __P4 p4)
//	{
//		RTTIMethodDescriptor* pm = findMethodfunc< stInvokeCallHelper<int, __RT, __P1, __P2, __P3, __P4>::new4 >("new4", 0, RTTI_MTH_CONSTRUCTOR);
//
//		if(pm)
//		{
//			bool boret = false;
//
//			if(pm->invoke(boret, (stInvokeCallHelper<int, __RT, __P1, __P2, __P3, __P4>*)NULL, (void**)&retobj, p1, p2, p3, p4))
//			{
//				__RT* tmpobj = NULL;
//				tmpobj = (__RT*)ConvertClassPtr((void*)retobj, this, __RTTITypeOfPtr((__RT*)NULL));
//
//				if(tmpobj == NULL)
//				{
//					tmpobj = retobj;
//				}
//
//				retobj = tmpobj;
//				return boret;
//			}
//		}
//		else
//		{
//			char szbuff[512];
//			g_logger.debug("找不到与函数原型 %s 匹配的构造函数!", RTTIFuncTypeGetDis((stInvokeCallHelper<int, __RT, __P1, __P2, __P3, __P4>::new4)NULL, szbuff, this, "new4"));
//		}
//
//		return false;
//	}

	RTTIClassDescriptor** getBaseClasses()
	{
		return baseClasses;
	}


	int getNumberOfBaseClasses()
	{
		return nBaseClasses;
	}

	void* ConvertClassPtr(void* psrc, RTTIClassDescriptor* src_cls, RTTIClassDescriptor* dst_cls);


	void* ConvertClassPtr(void* p, int ibase);


	bool IsKindOf(RTTIClassDescriptor* kindclass, int* offset = NULL);


	RTTIMethodDescriptor** getMethods()
	{
		return methods;
	}


	int getNumberOfMethods()
	{
		return nMethods;
	}


	RTTIFieldDescriptor*  findField(char const* name, int nbegin = 0, int flag = 0);
	RTTIFieldDescriptor* findFieldByAliasName(char const* name, int nbegin = 0, int flag = 0, bool bocasestr = false);

	RTTIMethodDescriptor* findMethod(char const* name, int nbegin = 0, int flag = 0);
	RTTIMethodDescriptor* findMethodByAliasName(char const* name, int nbegin = 0, int flag = 0, bool bocasestr = false);

	template<class __func_type>
	RTTIMethodDescriptor* findMethodfunc(char const* name, int nbegin = 0, int flag = 0)
	{
		__func_type tmpfunc = (__func_type)NULL;
		RTTIMethodDescriptor* pm = findMethod(name, nbegin, flag);

		while(pm != NULL)
		{
//			if(RTTIFuncTypeCheckIdx(tmpfunc) == pm->getType()->get_check_idx_r())
			{
				return pm;
			}

//			pm = findMethod(name, pm->index + 1, flag);
		}

		return NULL;
	};
	template<class __proc_type>
	RTTIMethodDescriptor* findMethodproc(char const* name, int nbegin = 0, int flag = 0)
	{
		__proc_type tmpproc = (__proc_type)NULL;
		RTTIMethodDescriptor* pm = findMethod(name, nbegin, flag);

		while(pm != NULL)
		{
//			if(RTTIProcTypeCheckIdx(tmpproc) == pm->getType()->get_check_idx())
			{
				return pm;
			}

//			pm = findMethod(name, pm->index + 1, flag);
		}

		return NULL;
	};

	RTTIClassDescriptor(char const* name, int size,
	                    RTTIDescribeFieldsFunc  describeFieldsFunc,
	                    RTTIDescribeMethodsFunc describeMethodsFunc,

	                    int flags, int sortfieldtype = eSortNone, char const* aliasname = NULL,
	                    unsigned short int userdefine = RTTI_STRUCT);


	RTTIClassDescriptor(char const* name, int size, int flags, int sortfieldtype = eSortNone, char const* aliasname = NULL, unsigned short int userdefine = RTTI_STRUCT);


	~RTTIClassDescriptor();

protected:
	friend class RTTIRepository;
	friend class RTTIBfdRepository;

	RTTIClassDescriptor*    next;
	RTTIClassDescriptor*    collisionChain;
	RTTIClassDescriptor*    collisionAliasChain;

	RTTIMethodDescriptor*   methodList;
	RTTIMethodDescriptor**  methods;
	int                     nMethods;

	RTTIFieldDescriptor*    fieldList;
	RTTIFieldDescriptor**   fields;
	int                     nFields;

	int                     flags;
	int                     size;

	bool                    initialized;

	char const*             name;
	char const*				aliasname;
	unsigned                hashCode;
	unsigned                hashAliasCode;

	int                     nBaseClasses;
	RTTIClassDescriptor**   baseClasses;
	RTTIFieldDescriptor**	baseClassesFields;

	void buildClassDescriptor(int sortfieldtype);
};


class  RTTIMethodType : public RTTIType
{
public:
	char* getTypeName(char* buf);

	char* getMethodDeclaration(char* buf, char const* name);

	static  __inline int s_get_check_idx()
	{
		return (int)(0);
	};
	static  __inline int s_get_check_idx_rc()
	{
		return (int)(0);
	};
	static  __inline int s_get_check_idx_r()
	{
		return (int)(0);
	};
	static  __inline int s_get_check_idx_c()
	{
		return (int)(0);
	};

	virtual  int get_check_idx()
	{
		return (int)(0);
	};
	virtual  int get_check_idx_rc()
	{
		return (int)(0);
	};
	virtual  int get_check_idx_r()
	{
		return (int)(0);
	};
	virtual  int get_check_idx_c()
	{
		return (int)(0);
	};

	RTTIClassDescriptor* getClass()
	{
		return methodClass;
	}

	RTTIType* getReturnType()
	{
		return returnType;
	}

	RTTIType** getParameterTypes()
	{
		return paramTypes;
	}

	int getNumberOfParameters()
	{
		return nParams;
	}

	RTTIMethodType() : RTTIType(RTTI_METHOD)
	{
		returnType = NULL;
		methodobj = NULL;
		memset(paramTypes, 0, sizeof(paramTypes));
	}
	~RTTIMethodType()
	{
		for(int i = 0; i < RTTI_MAX_PARAMETERS; i++)
		{
			if(paramTypes[i])
			{
				paramTypes[i]->destroy();
				paramTypes[i] = NULL;
			}
		}

		if(returnType)
		{
			returnType->destroy();
		}
	}
protected:
	friend class RTTIMethodDescriptor;
	friend class RTTIBfdRepository;

	RTTIType*  returnType;
	int        nParams;
	RTTIType* paramTypes[RTTI_MAX_PARAMETERS];
	RTTIClassDescriptor* methodClass;
	RTTIMethodDescriptor* methodobj;
};

/*************************************************************
* 说明 :
*************************************************************/
class RTTIMethodDescriptor
{
public:

	char const* getName()
	{
		return name;
	}
	char const* getAliasName()
	{
		return aliasname;
	}

	RTTIMethodType* getType()
	{
		return type;
	}


	int getFlags()
	{
		return flags;
	}

	char* getMethodDeclaration(char* buf)
	{
		return type->getMethodDeclaration(buf, aliasname);
	}
//
//	template<class __C>
//	bool vinvoke(__C* obj)
//	{
//		if(type->returnType->getTag() != RTTIType::RTTI_VOID || ((int)RTTIProcType0<__C>::s_get_check_idx()) != type->get_check_idx())
//		{
//			char szbuff[512];
//			char szbuff1[512];
//			RTTIProcType0<__C> tmpfunc(NULL, type->methodClass);
//			g_logger.error("RTTIMethod.invokeProc0-> %s<>%s  函数原型不匹配!", getMethodDeclaration(szbuff), tmpfunc.getMethodDeclaration(szbuff1, aliasname));
//			return false;
//		}
//
//		((RTTIProcType0<__C>*)(type))->invoke(obj);
//		return true;
//	};
//	template<class __C, class __P1>
//	bool vinvoke(__C* obj, __P1 p1)
//	{
//		if(type->returnType->getTag() != RTTIType::RTTI_VOID || ((int)RTTIProcType1<__C, __P1>::m_class_check_idx) != type->get_check_idx())
//		{
//			char szbuff[512];
//			char szbuff1[512];
//			RTTIProcType1<__C, __P1> tmpfunc(NULL, type->methodClass);
//			g_logger.error("RTTIMethod.invokeProc1-> %s<>%s  函数原型不匹配!", getMethodDeclaration(szbuff), tmpfunc.getMethodDeclaration(szbuff1, aliasname));
//			return false;
//		}
//
//		((RTTIProcType1<__C, __P1>*)(type))->invoke(obj, p1);
//		return true;
//	};
//	template<class __C, class __P1, class __P2>
//	bool vinvoke(__C* obj, __P1 p1, __P2 p2)
//	{
//		if(type->returnType->getTag() != RTTIType::RTTI_VOID || ((int)RTTIProcType2<__C, __P1, __P2>::s_get_check_idx()) != type->get_check_idx())
//		{
//			char szbuff[512];
//			char szbuff1[512];
//			RTTIProcType2<__C, __P1, __P2> tmpfunc(NULL, type->methodClass);
//			g_logger.error("RTTIMethod.invokeProc2-> %s<>%s  函数原型不匹配!", getMethodDeclaration(szbuff), tmpfunc.getMethodDeclaration(szbuff1, aliasname));
//			return false;
//		}
//
//		((RTTIProcType2<__C, __P1, __P2>*)(type))->invoke(obj, p1, p2);
//		return true;
//	};
//	template<class __C, class __P1, class __P2, class __P3>
//	bool vinvoke(__C* obj, __P1 p1, __P2 p2, __P3 p3)
//	{
//		if(type->returnType->getTag() != RTTIType::RTTI_VOID || ((int)RTTIProcType3<__C, __P1, __P2, __P3>::s_get_check_idx()) != type->get_check_idx())
//		{
//			char szbuff[512];
//			char szbuff1[512];
//			RTTIProcType3<__C, __P1, __P2, __P3> tmpfunc(NULL, type->methodClass);
//			g_logger.error("RTTIMethod.invokeProc3-> %s<>%s  函数原型不匹配!", getMethodDeclaration(szbuff), tmpfunc.getMethodDeclaration(szbuff1, aliasname));
//			return false;
//		}
//
//		((RTTIProcType3<__C, __P1, __P2, __P3>*)(type))->invoke(obj, p1, p2, p3);
//		return true;
//	};
//	template<class __C, class __P1, class __P2, class __P3, class __P4>
//	bool vinvoke(__C* obj, __P1 p1, __P2 p2, __P3 p3, __P4 p4)
//	{
//		if(type->returnType->getTag() != RTTIType::RTTI_VOID || ((int)RTTIProcType4<__C, __P1, __P2, __P3, __P4>::s_get_check_idx()) != type->get_check_idx())
//		{
//			char szbuff[512];
//			char szbuff1[512];
//			RTTIProcType4<__C, __P1, __P2, __P3, __P4> tmpfunc(NULL, type->methodClass);
//			g_logger.error("RTTIMethod.invokeProc4-> %s<>%s  函数原型不匹配!", getMethodDeclaration(szbuff), tmpfunc.getMethodDeclaration(szbuff1, aliasname));
//			return false;
//		}
//
//		((RTTIProcType4<__C, __P1, __P2, __P3, __P4>*)(type))->invoke(obj, p1, p2, p3, p4);
//		return true;
//	};
//	template<class __C, class __P1, class __P2, class __P3, class __P4, class __P5>
//	bool vinvoke(__C* obj, __P1 p1, __P2 p2, __P3 p3, __P4 p4, __P5 p5)
//	{
//		if(type->returnType->getTag() != RTTIType::RTTI_VOID || ((int)RTTIProcType5<__C, __P1, __P2, __P3, __P4, __P5>::s_get_check_idx()) != type->get_check_idx())
//		{
//			char szbuff[512];
//			char szbuff1[512];
//			RTTIProcType5<__C, __P1, __P2, __P3, __P4, __P5> tmpfunc(NULL, type->methodClass);
//			g_logger.error("RTTIMethod.invokeProc5-> %s<>%s  函数原型不匹配!", getMethodDeclaration(szbuff), tmpfunc.getMethodDeclaration(szbuff1, aliasname));
//			return false;
//		}
//
//		((RTTIProcType5<__C, __P1, __P2, __P3, __P4, __P5>*)(type))->invoke(obj, p1, p2, p3, p4, p5);
//		return true;
//	};
//	template<class __C, class __RT>
//	bool invoke(__RT& rt, __C* obj)
//	{
//		if(type->returnType->getTag() == RTTIType::RTTI_VOID || ((int)RTTIFuncType0<__RT, __C>::s_get_check_idx_r()) != type->get_check_idx_r())
//		{
//			char szbuff[512];
//			char szbuff1[512];
//			RTTIFuncType0<__RT, __C> tmpfunc(NULL, type->methodClass);
//			g_logger.error("RTTIMethod.invokeFunc0-> %s<>%s  函数原型不匹配!", getMethodDeclaration(szbuff), tmpfunc.getMethodDeclaration(szbuff1, aliasname));
//			return false;
//		}
//
//		rt = ((RTTIFuncType0<__RT, __C>*)(type))->invoke(obj);
//		return true;
//	};
//	template<class __C, class __P1, class __RT>
//	bool invoke(__RT& rt, __C* obj, __P1 p1)
//	{
//		if(type->returnType->getTag() == RTTIType::RTTI_VOID || ((int)RTTIFuncType1<__RT, __C, __P1>::s_get_check_idx_r()) != type->get_check_idx_r())
//		{
//			char szbuff[512];
//			char szbuff1[512];
//			RTTIFuncType1<__RT, __C, __P1> tmpfunc(NULL, type->methodClass);
//			g_logger.error("RTTIMethod.invokeFunc1-> %s<>%s  函数原型不匹配!", getMethodDeclaration(szbuff), tmpfunc.getMethodDeclaration(szbuff1, aliasname));
//			return false;
//		}
//
//		rt = ((RTTIFuncType1<__RT, __C, __P1>*)(type))->invoke(obj, p1);
//		return true;
//	};
//	template<class __C, class __P1, class __P2, class __RT>
//	bool invoke(__RT& rt, __C* obj, __P1 p1, __P2 p2)
//	{
//		if(type->returnType->getTag() == RTTIType::RTTI_VOID || ((int)RTTIFuncType2<__RT, __C, __P1, __P2>::s_get_check_idx_r()) != type->get_check_idx_r())
//		{
//			char szbuff[512];
//			char szbuff1[512];
//			RTTIFuncType2<__RT, __C, __P1, __P2> tmpfunc(NULL, type->methodClass);
//			g_logger.error("RTTIMethod.invokeFunc2-> %s<>%s  函数原型不匹配!", getMethodDeclaration(szbuff), tmpfunc.getMethodDeclaration(szbuff1, aliasname));
//			return false;
//		}
//
//		rt = ((RTTIFuncType2<__RT, __C, __P1, __P2>*)(type))->invoke(obj, p1, p2);
//		return true;
//	};
//	template<class __C, class __P1, class __P2, class __P3, class __RT>
//	bool invoke(__RT& rt, __C* obj, __P1 p1, __P2 p2, __P3 p3)
//	{
//		if(type->returnType->getTag() == RTTIType::RTTI_VOID || ((int)RTTIFuncType3<__RT, __C, __P1, __P2, __P3>::s_get_check_idx_r()) != type->get_check_idx_r())
//		{
//			char szbuff[512];
//			char szbuff1[512];
//			RTTIFuncType3<__RT, __C, __P1, __P2, __P3> tmpfunc(NULL, type->methodClass);
//			g_logger.error("RTTIMethod.invokeFunc3-> %s<>%s  函数原型不匹配!", getMethodDeclaration(szbuff), tmpfunc.getMethodDeclaration(szbuff1, aliasname));
//			return false;
//		}
//
//		rt = ((RTTIFuncType3<__RT, __C, __P1, __P2, __P3>*)(type))->invoke(obj, p1, p2, p3);
//		return true;
//	};
//	template<class __C, class __P1, class __P2, class __P3, class __P4, class __RT>
//	bool invoke(__RT& rt, __C* obj, __P1 p1, __P2 p2, __P3 p3, __P4 p4)
//	{
//		if(type->returnType->getTag() == RTTIType::RTTI_VOID || ((int)RTTIFuncType4<__RT, __C, __P1, __P2, __P3, __P4>::s_get_check_idx_r()) != type->get_check_idx_r())
//		{
//			char szbuff[512];
//			char szbuff1[512];
//			RTTIFuncType4<__RT, __C, __P1, __P2, __P3, __P4> tmpfunc(NULL, type->methodClass);
//			g_logger.error("RTTIMethod.invokeFunc4-> %s<>%s  函数原型不匹配!", getMethodDeclaration(szbuff), tmpfunc.getMethodDeclaration(szbuff1, aliasname));
//			return false;
//		}
//
//		rt = ((RTTIFuncType4<__RT, __C, __P1, __P2, __P3, __P4>*)(type))->invoke(obj, p1, p2, p3, p4);
//		return true;
//	};
//	template<class __C, class __P1, class __P2, class __P3, class __P4, class __P5, class __RT>
//	bool invoke(__RT& rt, __C* obj, __P1 p1, __P2 p2, __P3 p3, __P4 p4, __P5 p5)
//	{
//		if(type->returnType->getTag() == RTTIType::RTTI_VOID || ((int)RTTIFuncType5<__RT, __C, __P1, __P2, __P3, __P4, __P5>::s_get_check_idx_r()) != type->get_check_idx_r())
//		{
//			char szbuff[512];
//			char szbuff1[512];
//			RTTIFuncType5<__RT, __C, __P1, __P2, __P3, __P4, __P5> tmpfunc(NULL, type->methodClass);
//			g_logger.error("RTTIMethod.invokeFunc5-> %s<>%s  函数原型不匹配!", getMethodDeclaration(szbuff), tmpfunc.getMethodDeclaration(szbuff1, aliasname));
//			return false;
//		}
//
//		rt = ((RTTIFuncType5<__RT, __C, __P1, __P2, __P3, __P4, __P5>*)(type))->invoke(obj, p1, p2, p3, p4, p5);
//		return true;
//	};

	RTTIClassDescriptor* getDeclaringClass()
	{
		return type->getClass();
	}

	RTTIMethodDescriptor(char const* name, int flags, RTTIMethodType* type, char const* aliasname = NULL)
	{
		this->name = name;
		this->aliasname = aliasname;

		if(!this->aliasname)
		{
			this->aliasname = name;
		}

		this->flags = flags;
		this->type = type;
		type->methodobj = this;
		next = NULL;
		chain = &next;
	}

	RTTIMethodDescriptor& operator, (RTTIMethodDescriptor& method)
	{
		*chain = &method;
		chain = &method.next;
		return *this;
	}


	~RTTIMethodDescriptor()
	{
		type->destroy();
	}

	int getIndex()
	{
		return index;
	}
	bool isOverloaded()
	{
		return (flags & RTTI_MTH_OVERLOADED) != 0;
	}
	bool isVirtual()
	{
		return (flags & RTTI_MTH_VIRTUAL) != 0;
	}
	bool isAbstract()
	{
		return (flags & RTTI_MTH_ABSTRACT) != 0;
	}
	bool isStatic()
	{
		return (flags & RTTI_MTH_STATIC) != 0;
	}
protected:
	friend class RTTIType;
	friend class RTTIClassDescriptor;
	friend class RTTIBfdRepository;


	int             flags;
	int             index;
	RTTIMethodType* type;
	char const*     name;
	char const* aliasname;

	RTTIMethodDescriptor*  next;
	RTTIMethodDescriptor** chain;
};






class  RTTIPtrType : public RTTIType
{
public:
	RTTIPtrType(RTTIType* ptrType) : RTTIType(RTTI_PTR)
	{
		this->ptrType = ptrType;
		RTTIType* tempptrType = this;
		m_nPtrLevel = 0;

		while(tempptrType->isPointer())
		{
			m_nPtrLevel++;
			tempptrType = ((RTTIPtrType*)tempptrType)->ptrType;
		}
	}
	char* getTypeName(char* buf);

	~RTTIPtrType()
	{
		if(m_nPtrLevel > 0)
		{
			ptrType->destroy();
		}
	}

	virtual RTTIType* getPtrDataType(int nBackLevel = 0x7fff)
	{
		RTTIType* tempptrType = this;

		if(nBackLevel > 0)
		{
			nBackLevel = nBackLevel > m_nPtrLevel ? m_nPtrLevel : nBackLevel;

			while(nBackLevel > 0)
			{
				tempptrType = ((RTTIPtrType*)tempptrType)->ptrType;
				nBackLevel--;
			}
		}

		return tempptrType;
	}
protected:
	RTTIType* ptrType;
};


class  RTTIArrayType : public RTTIType
{
public:
	RTTIArrayType(RTTIType* elemType, int nElems) : RTTIType(RTTI_ARRAY)
	{
		this->elemType = elemType;
		this->nElems = nElems;
	}
	~RTTIArrayType()
	{
		elemType->destroy();
	}
	char* getTypeName(char* buf);

	int  getArraySize()
	{
		return nElems;
	}

	RTTIType* getElementType()
	{
		return elemType;
	}

protected:
	RTTIType* elemType;
	int       nElems;
};


class  RTTIDerivedType : public RTTIType
{
public:
	RTTIDerivedType(RTTIClassDescriptor* baseClass) : RTTIType(RTTI_DERIVED)
	{
		this->baseClass = baseClass;
	}

	RTTIClassDescriptor* getBaseClass()
	{
		return baseClass;
	}

	char* getTypeName(char* buf);

protected:
	RTTIClassDescriptor* baseClass;
};

/*------------- typedecl.h
*
* Copyright (C): www.7cool.cn (2010)
* Author       :
* Version      : V1.01
* Date         : 2010/11/26 9:36:14
*
*/
/*************************************************************
*
*************************************************************/
#pragma once
#include "define/define.h"
#include <typeinfo>
#include"utils/rtti/type.h"

/*************************************************************/
#define RTTI_FIELD_N(x, flags, aname) \
	*new RTTIFieldDescriptor(#x, (char*)&tempthis->x-(char*)tempthis, sizeof(tempthis->x), flags, RTTITypeOf(tempthis->x),#aname)

#define RTTI_ARRAY_N(x, flags, aname) \
	*new RTTIFieldDescriptor(#x, (char*)&tempthis->x-(char*)tempthis, sizeof(tempthis->x), flags, \
	new RTTIArrayType(RTTITypeOf(*tempthis->x), sizeof(tempthis->x)/sizeof(*tempthis->x)),#aname)


#define RTTI_BASE_CLASS_N(BC, flags, aname)  \
	*new RTTIFieldDescriptor(#BC, (char*)((BC*)tempthis) - (char*)tempthis, sizeof(BC), flags, \
	new RTTIDerivedType(__RTTITypeOfPtr((BC*)NULL)),#BC)

#define RTTI_FIELD(x, flags)				RTTI_FIELD_N(x,flags,x)
#define RTTI_ARRAY(x, flags)				RTTI_ARRAY_N(x, flags,x)


#define RTTI_BASE_CLASS(BC, flags)			RTTI_BASE_CLASS_N(BC, flags,BC)


#include <memory>

#define RTTI_DESCRIBE(T,components,methods) \
	static RTTIClassDescriptor TRTTIDescriptorClassObj; \
	 static RTTIClassDescriptor* RTTI() { \
	return &TRTTIDescriptorClassObj; \
	} \
	 static RTTIClassDescriptor* RTTIGetClass() { \
	return &TRTTIDescriptorClassObj; \
	} \
	template<class CT> \
	bool ClassIs() { \
	return RTTIGetClass()==__RTTITypeOfPtr((CT*)NULL);\
	} \
	bool ClassNameIs(char* cname) { \
	char buf[256]={0}; \
	RTTIClassDescriptor* cls=RTTIGetClass(); \
	if (cls) \
	{ \
	cls->getTypeName(buf); \
	return strcmp(buf,cname)==0; \
	} \
	return false; \
	} \
	RTTIClassDescriptor* RTTIFindClass() 	\
	{ 	\
	RTTIRepository* repo = RTTIRepository::getInstance(); 	\
	return repo?repo->findClass(typeid(*this).name()):NULL;		\
	} 	\
	RTTIFieldDescriptor* RTTIDescribeFields() { \
	  	\
	T* tempthis=((T*)0x0000ffff); 	\
	return (components); \
	} \
	typedef T self; \
	__inline static RTTIMethodDescriptor* RTTIDescribeMethods() { \
	return (methods); \
	}  \
	template<class CT> \
	bool IsKindOf() 	\
	{  	\
	RTTIClassDescriptor* kindclass=__RTTITypeOfPtr((CT*)NULL); 	\
	RTTIClassDescriptor* thisclass=RTTIGetClass();  \
	if (thisclass){  \
	return thisclass->IsKindOf(kindclass);  \
	}  \
	return false;  \
	}  \


#define RTTI_DESCRIBE_V(T,components,methods) \
	static RTTIClassDescriptor TRTTIDescriptorClassObj; \
	__inline static RTTIClassDescriptor* RTTI() { \
	return &TRTTIDescriptorClassObj; \
	} \
	virtual RTTIClassDescriptor* RTTIGetClass() 	\
	{ 	\
	return &TRTTIDescriptorClassObj;	\
	} 	\
	template<class CT> \
	bool ClassIs() { \
	return RTTIGetClass()==__RTTITypeOfPtr((CT*)NULL);\
	} \
	bool ClassNameIs(char* cname) { \
	char buf[256]={0}; \
	RTTIClassDescriptor* cls=RTTIGetClass(); \
	if (cls) \
	{ \
	cls->getTypeName(buf); \
	return strcmp(buf,cname)==0; \
	} \
	return false; \
	} \
	RTTIClassDescriptor* RTTIFindClass() 	\
	{ 	\
	return RTTIGetClass(); \
	} 	\
	RTTIFieldDescriptor* RTTIDescribeFields() { \
	  	\
	T* tempthis=((T*)0x0000ffff); 	\
	return (components); \
	} \
	typedef T self; \
	__inline static RTTIMethodDescriptor* RTTIDescribeMethods() { \
	return (methods); \
	}  \
	template<class CT> \
	bool IsKindOf() 	\
	{  	\
	RTTIClassDescriptor* kindclass=__RTTITypeOfPtr((CT*)NULL); 	\
	RTTIClassDescriptor* thisclass=RTTIGetClass();  \
	if (thisclass){  \
	return thisclass->IsKindOf(kindclass);  \
	}  \
	return false;  \
	}  \

#if defined(__GNUC__) && __GNUC_MINOR__ < 96




#define RTTI_BASE_REGISTER_N(T,TNAME,flags,sortfieldtype)	\
	static RTTIFieldDescriptor* RTTIDescribeFieldsOf##T() { \
	return ((T*)(NULL))->RTTIDescribeFields(); \
	} \
	static RTTIMethodDescriptor* RTTIDescribeMethodsOf##T() { \
	return ((T*)(NULL))->RTTIDescribeMethods(); \
	} \
	RTTIClassDescriptor T::TRTTIDescriptorClassObj(#T, sizeof(T), &RTTIDescribeFieldsOf##T, \
	&RTTIDescribeMethodsOf##T,flags,sortfieldtype,#TNAME); \
	RTTIClassDescriptor* RTTIGetClassDescriptor(T* ) {   \
	return T::RTTI(); \
	}

template<class __P>
inline RTTIClassDescriptor* __RTTITypeOfPtr(__P const*)
{
	extern RTTIClassDescriptor* RTTIGetClassDescriptor(__P*);
	return RTTIGetClassDescriptor((__P*)0);
}

template<class __T>
inline RTTIType* RTTITypeOf(__T&)
{
	extern RTTIClassDescriptor* RTTIGetClassDescriptor(__T*);
	return RTTIGetClassDescriptor((__T*)0);
}

#else

#define RTTI_BASE_REGISTER_N(T,TNAME,flags,sortfieldtype)	\
	static RTTIFieldDescriptor* RTTIDescribeFieldsOf##T() { \
	return ((T*)(NULL))->RTTIDescribeFields(); \
	} \
	static RTTIMethodDescriptor* RTTIDescribeMethodsOf##T() { \
	return ((T*)(NULL))->RTTIDescribeMethods(); \
	} \
	RTTIClassDescriptor T::TRTTIDescriptorClassObj(#T, sizeof(T), &RTTIDescribeFieldsOf##T, \
	&RTTIDescribeMethodsOf##T,flags,sortfieldtype,#TNAME); \
	template<> \
	RTTIClassDescriptor* RTTIClassDescriptorHelper<T>::getClassDescriptor() {   \
	return T::RTTI(); \
	}

template<class __T>
class RTTIClassDescriptorHelper
{
public:
	static RTTIClassDescriptor* getClassDescriptor();
};

template<class __P>
inline RTTIClassDescriptor* __RTTITypeOfPtr(__P const*)
{
	return RTTIClassDescriptorHelper<__P>::getClassDescriptor();
}

template<class __T>
inline RTTIType* RTTITypeOf(__T&)
{
	return RTTIClassDescriptorHelper<__T>::getClassDescriptor();
}

#endif

#define RTTI_DESCRIBE_STRUCT(T, components)		RTTI_DESCRIBE(T, &components ,NULL)
#define RTTI_DESCRIBE_STRUCT_V(T, components)	RTTI_DESCRIBE_V(T, &components ,NULL)

#define RTTI_DESCRIBE_CLASS(T, components,methods)		RTTI_DESCRIBE(T, &components ,&methods)
#define RTTI_DESCRIBE_CLASS_V(T, components,methods)	RTTI_DESCRIBE_V(T, &components ,&methods)

#define RTTI_REGISTER_STRUCT(T)			RTTI_BASE_REGISTER_N(T,T,0,eSortNone)
#define RTTI_REGISTER_STRUCT_N(T,TNAME)	RTTI_BASE_REGISTER_N(T,TNAME,0,eSortNone)
#define RTTI_REGISTER_CLASS			RTTI_REGISTER_STRUCT
#define RTTI_REGISTER_CLASS_N		RTTI_REGISTER_STRUCT_N

#define RTTI_BASE_REGISTER_STRUCT(T)			RTTI_BASE_REGISTER_N(T,T,0,eSortNone)
#define RTTI_BASE_REGISTER_STRUCT_N(T,TNAME)	RTTI_BASE_REGISTER_N(T,TNAME,0,eSortNone)
#define RTTI_BASE_REGISTER_CLASS			RTTI_BASE_REGISTER_STRUCT
#define RTTI_BASE_REGISTER_CLASS_N			RTTI_BASE_REGISTER_STRUCT_N


template<>
inline RTTIType* RTTITypeOf(char&)
{
	return &RTTIType::charType;
}
template<>
inline RTTIType* RTTITypeOf(unsigned char&)
{
	return &RTTIType::ucharType;
}
template<>
inline RTTIType* RTTITypeOf(signed char&)
{
	return &RTTIType::scharType;
}


template<>
inline RTTIType* RTTITypeOf(short&)
{
	return &RTTIType::shortType;
}
template<>
inline RTTIType* RTTITypeOf(unsigned short&)
{
	return &RTTIType::ushortType;
}
template<>
inline RTTIType* RTTITypeOf(int&)
{
	return &RTTIType::intType;
}
template<>
inline RTTIType* RTTITypeOf(unsigned int&)
{
	return &RTTIType::uintType;
}

// #ifdef WIN32//zc:fixed windows中要用 不知道为什么linux中不用
// 
// template<>
// inline RTTIType* RTTITypeOf(int64_t&)
// {
// 	return &RTTIType::longType;
// }
// 
// template<>
// inline RTTIType* RTTITypeOf(uint64_t&)
// {
// 	return &RTTIType::ulongType;
// }
// 
// #endif

template<>
inline RTTIType* RTTITypeOf(uint64_t&)
{
	return &RTTIType::ui64Type;
}

template<>
inline RTTIType* RTTITypeOf(int64_t&)
{
	return &RTTIType::i64Type;
}

template<>
inline RTTIType* RTTITypeOf(float&)
{
	return &RTTIType::floatType;
}
template<>
inline RTTIType* RTTITypeOf(double&)
{
	return &RTTIType::doubleType;
}
template<>
inline RTTIType* RTTITypeOf(bool&)
{
	return &RTTIType::boolType;
}

inline RTTIClassDescriptor* __RTTITypeOfPtr(char*)
{
	return (RTTIClassDescriptor*)&RTTIType::charType;
}
inline RTTIType* RTTITypeOfPtr(char*)
{
	return &RTTIType::charType;
}

inline RTTIClassDescriptor* __RTTITypeOfPtr(unsigned char*)
{
	return (RTTIClassDescriptor*)&RTTIType::ucharType;
}
inline RTTIType* RTTITypeOfPtr(unsigned char*)
{
	return &RTTIType::ucharType;
}

inline RTTIClassDescriptor* __RTTITypeOfPtr(signed char*)
{
	return (RTTIClassDescriptor*)&RTTIType::scharType;
}
inline RTTIType* RTTITypeOfPtr(signed char*)
{
	return &RTTIType::scharType;
}


inline RTTIClassDescriptor* __RTTITypeOfPtr(short*)
{
	return (RTTIClassDescriptor*)&RTTIType::shortType;
}
inline RTTIType* RTTITypeOfPtr(short*)
{
	return &RTTIType::shortType;
}

inline RTTIClassDescriptor* __RTTITypeOfPtr(unsigned short*)
{
	return (RTTIClassDescriptor*)&RTTIType::ushortType;
}
inline RTTIType* RTTITypeOfPtr(unsigned short*)
{
	return &RTTIType::ushortType;
}

inline RTTIClassDescriptor* __RTTITypeOfPtr(int*)
{
	return (RTTIClassDescriptor*)&RTTIType::intType;
}
inline RTTIType* RTTITypeOfPtr(int*)
{
	return &RTTIType::intType;
}

inline RTTIClassDescriptor* __RTTITypeOfPtr(unsigned int*)
{
	return (RTTIClassDescriptor*)&RTTIType::uintType;
}
inline RTTIType* RTTITypeOfPtr(unsigned int*)
{
	return &RTTIType::uintType;
}
// #ifdef WIN32//在linux平台下,long和int64_t是一样的;win32下，long是4字节的.
// inline RTTIClassDescriptor* __RTTITypeOfPtr(long*)
// {
// 	return (RTTIClassDescriptor*)&RTTIType::longType;
// }
// inline RTTIType* RTTITypeOfPtr(long*)
// {
// 	return &RTTIType::longType;
// }
// 
// inline RTTIClassDescriptor* __RTTITypeOfPtr(uint64_t*)
// {
// 	return (RTTIClassDescriptor*)&RTTIType::ulongType;
// }
// inline RTTIType* RTTITypeOfPtr(uint64_t*)
// {
// 	return &RTTIType::ulongType;
// }
// #endif//end win32;

__inline RTTIClassDescriptor* __RTTITypeOfPtr(int64_t*)
{
	return (RTTIClassDescriptor*)&RTTIType::i64Type;
}
inline RTTIType* RTTITypeOfPtr(int64_t*)
{
	return &RTTIType::i64Type;
}

inline RTTIClassDescriptor* __RTTITypeOfPtr(uint64_t*)
{
return (RTTIClassDescriptor*)&RTTIType::ui64Type;
}

inline RTTIType* RTTITypeOfPtr(uint64_t*)
{
return &RTTIType::ui64Type;
}



inline RTTIClassDescriptor* __RTTITypeOfPtr(float*)
{
	return (RTTIClassDescriptor*)&RTTIType::floatType;
}
inline RTTIType* RTTITypeOfPtr(float*)
{
	return &RTTIType::floatType;
}

inline RTTIClassDescriptor* __RTTITypeOfPtr(double*)
{
	return (RTTIClassDescriptor*)&RTTIType::doubleType;
}
inline RTTIType* RTTITypeOfPtr(double*)
{
	return &RTTIType::doubleType;
}

inline RTTIClassDescriptor* __RTTITypeOfPtr(bool*)
{
	return (RTTIClassDescriptor*)&RTTIType::boolType;
}
inline RTTIType* RTTITypeOfPtr(bool*)
{
	return &RTTIType::boolType;
}

inline RTTIClassDescriptor* __RTTITypeOfPtr(void*)
{
	return (RTTIClassDescriptor*)&RTTIType::voidType;
}
inline RTTIType* RTTITypeOfPtr(void*)
{
	return &RTTIType::voidType;
}


#if defined(__GNUC__) && __GNUC_MINOR__ < 96

template<class __P>
inline RTTIType* RTTITypeOfPtr(__P const*const*)
{
	extern RTTIClassDescriptor* RTTIGetClassDescriptor(__P*);
	return new RTTIPtrType(RTTIGetClassDescriptor((__P*)0));
}

#else

template<class __P>
inline RTTIType* RTTITypeOfPtr(__P const*const*)
{
	return new RTTIPtrType(__RTTITypeOfPtr((__P*)NULL));
}

#endif



class RTTIFieldDescriptor
{
public:

	char const* getName()
	{
		return name;
	}
	char const* getAliasName()
	{
		return aliasname;
	}

	void fixPtrFieldName(char* fixname, int nBackLevel = 0x7fff)
	{
		if(getType()->isPointer() && nBackLevel >= 0)
		{
			nBackLevel = nBackLevel > getType()->getPtrLevel() ? getType()->getPtrLevel() : nBackLevel;
			memset(fixname, '*', nBackLevel);
		}

		size_t nlen = strlen(name);
		strncpy_q(&fixname[nBackLevel], name, nlen);
		fixname[nlen+nBackLevel] = 0;
	}


	void setValue(void* obj, void* buf, int nmaxsize)
	{
		memcpy(getPtr(obj), buf, safe_min(size, nmaxsize));
	}


	void getValue(void* obj, void* buf, int nmaxsize)
	{
		memcpy(buf, getPtr(obj),  safe_min(size, nmaxsize));
	}

	RTTIClassDescriptor* getDeclaringClass()
	{
		return declaringClass;
	}

	int getOffset()
	{
		return offs;
	}
	void* getPtr(void* p)
	{
		if(!(flags & RTTI_FLD_STATIC))
		{
			return ((void*)(((uint8_t*)p) + offs));
		}
		else
		{
			return ((void*)(int64_t)(offs));
		}
	}

	int getSize()
	{
		return size;
	}


	RTTIType* getType()
	{
		return type;
	}


	int getFlags()
	{
		return flags;
	}

	RTTIFieldDescriptor(char const* name, int offs, int size, int flags, RTTIType* type, char const* aliasname = NULL)
	{
		this->name = name;
		this->aliasname = aliasname;

		if(!this->aliasname)
		{
			this->aliasname = name;
		}

		this->offs = offs;
		this->size = size;
		this->type = type;
		this->flags = flags;
		next = NULL;
		chain = &next;
	}


	RTTIFieldDescriptor& operator, (RTTIFieldDescriptor& field)
	{
		*chain = &field;
		chain = &field.next;
		return *this;
	}


	int getIndex()
	{
		return index;
	}

	~RTTIFieldDescriptor()
	{
		type->destroy();
	}
protected:
	friend class RTTIType;
	friend class RTTIClassDescriptor;
	friend class RTTIBfdRepository;

	int         flags;
	int         index;
	RTTIType*   type;
	int         offs;
	int         size;
	char const* name;
	char const* aliasname;

	RTTIClassDescriptor*  declaringClass;

	RTTIFieldDescriptor*  next;
	RTTIFieldDescriptor** chain;
};

#include "define/noncopyable.h"
#include "thread/lockObj.h"

/*************************************************************/
const int RTTI_CLASS_HASH_SIZE = 1013;

class  RTTIRepository: public CIntLock
{
public:
	RTTIClassDescriptor* getFirstClass()
	{
		return classes;
	}

	RTTIClassDescriptor*               findClass(char const* pclassname, bool bocasestr = false);
	RTTIClassDescriptor*               findClassByAliasName(char const* pAliasName, bool bocasestr = false);
#ifdef USE_RTTI

	*
	RTTIClassDescriptor*               findClass(class type_info const& tinfo, bool bocasestr = false)
	{
		return findClass(tinfo.getName(), bocasestr);
	}
#endif

	static RTTIRepository* getInstance()
	{
		if(theRepository == NULL)
		{
			theRepository = new RTTIRepository;
		}

		return theRepository;
	}

	static RTTIRepository* instance_readonly()
	{
		return theRepository;
	}

	static void delInstance()
	{
		if(theRepository)
		{
		    DEFINE_DEL_ONE(theRepository);
			SAFE_DELETE(theRepository);

		}
	}


	bool addClass(RTTIClassDescriptor* cls);


	virtual bool load(char const* filePath);

	RTTIRepository(): CIntLock()
	{
		ZeroMemory(hashTable, sizeof(hashTable));
		ZeroMemory(hashAliasTable, sizeof(hashAliasTable));
		classes = NULL;
	}
	virtual ~RTTIRepository()
	{

	}

protected:
	static RTTIRepository* theRepository;
	RTTIClassDescriptor*  classes;
	RTTIClassDescriptor*  hashTable[RTTI_CLASS_HASH_SIZE];
	RTTIClassDescriptor*  hashAliasTable[RTTI_CLASS_HASH_SIZE];
};

typedef		RTTIRepository		RttiManage;
typedef		RTTIRepository		RttiM;



#define RTTI_FIELDNAME(pf,buf,objname)\
if (objname==NULL || objname[0]==0)\
	strcpy_q(buf,sizeof(buf)-1,pf->getAliasName());\
else\
	sprintf_q(buf,sizeof(buf)-1,"%s.%s",objname,pf->getAliasName());

//typedef bool (WINAPI* fnfieldfilter)(void* p, RTTIClassDescriptor* pclass, TiXmlElement* node, RTTIFieldDescriptor* pfield, const char* objname);
//bool SaveClassToXml(void* p, RTTIClassDescriptor* pclass, TiXmlElement* node, fnfieldfilter filter = NULL, const char* objname = NULL);
//bool InitClassFromXml(void* p, RTTIClassDescriptor* pclass, TiXmlElement* node, fnfieldfilter filter = NULL, const char* objname = NULL);


#endif //_TYPE_H__

