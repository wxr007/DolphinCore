#include"sql/rttiSqlHelper.h"



eDB_CLDTYPE ConvertRtti2Dbtype(RTTIFieldDescriptor* pfield)
{
	switch(pfield->getType()->getTag())
	{
	case RTTIType::RTTI_UNKNOWN:
		return DB_TYPEEND;
	case RTTIType::RTTI_VOID:
		return DB_TYPEEND;
	case RTTIType::RTTI_CHAR:
		return DB_BYTE;
	case RTTIType::RTTI_UCHAR:
		return DB_BYTE;
	case RTTIType::RTTI_SCHAR:
		return DB_BYTE;
	case RTTIType::RTTI_SHORT:
		return DB_WORD;
	case RTTIType::RTTI_USHORT:
		return DB_WORD;
	case RTTIType::RTTI_INT:
		return DB_DWORD;
	case RTTIType::RTTI_UINT:
		return DB_DWORD;
	case RTTIType::RTTI_LONG:
		return DB_DWORD;
	case RTTIType::RTTI_ULONG:
		return DB_DWORD;
	case RTTIType::RTTI_I64:
		return DB_QWORD;
	case RTTIType::RTTI_UI64:
		return DB_QWORD;
	case RTTIType::RTTI_ENUM:
		{
			switch(pfield->getSize())
			{
			case 1:
				return DB_BYTE;
			case 2:
				return DB_WORD;
			case 4:
				return DB_DWORD;
			default:
				return DB_TYPEEND;
			}
		}
		break;
	case RTTIType::RTTI_FLOAT:
		return DB_FLOAT;
	case RTTIType::RTTI_DOUBLE:
		return DB_DOUBLE;
	case RTTIType::RTTI_BOOL:
		{
			switch(pfield->getSize())
			{
			case 1:
				return DB_BYTE;
			case 2:
				return DB_WORD;
			case 4:
				return DB_DWORD;
			default:
				return DB_TYPEEND;
			}
		}
	case RTTIType::RTTI_ARRAY:
		{
			RTTIArrayType* parray = ((RTTIArrayType*)pfield->getType());

			switch(parray->getElementType()->getTag())
			{
			case RTTIType::RTTI_CHAR:
			case RTTIType::RTTI_SCHAR:
				{
					return DB_STR;
				}
			case RTTIType::RTTI_UCHAR:
				{
					return DB_BIN;
				}
			}
		}
	default:
		{
			return DB_TYPEEND;
		}
	}
}


bool CRttiDbDataLoader::MakedbCol(dbColMaker& maker, RTTIClassDescriptor* pclass, int srcdatasize)
{
	if(pclass)
	{
		RTTIClassDescriptor** pbases = pclass->getBaseClasses();
		int nbasecount = pclass->getNumberOfBaseClasses();
		if(pbases && nbasecount)
		{
			for(int i = 0; i < nbasecount; i++)
			{

				if(pbases[i])
				{
					if(!MakedbCol(maker, pbases[i], srcdatasize))
					{
						return false;
					}
				}
			}
		}

		RTTIFieldDescriptor** pfields = pclass->getFields();
		int nfieldcount = pclass->getNumberOfFields();

		if(pfields && nfieldcount)
		{
			for(int i = 0; i < nfieldcount; i++)
			{
				RTTIFieldDescriptor* pcurfield = pfields[i];

				if(pcurfield)
				{
					eDB_CLDTYPE dt = ConvertRtti2Dbtype(pcurfield);

					if(dt > DB_TYPEBEGIN && dt < DB_TYPEEND)
					{
						if(pcurfield->getFlags() & RTTI_FLD_STATIC)
						{
							maker.put(pcurfield->getAliasName(), dt, pcurfield->getSize(), 0, (unsigned char *)pcurfield->getPtr(NULL));
						}
						else
						{
							maker.put(pcurfield->getAliasName(), dt, pcurfield->getSize(), pcurfield->getOffset(), NULL);
						}
					}
					else if(pcurfield->getType()->getTag() == RTTIType::RTTI_STRUCT)
					{
						if(!MakedbCol(maker, (RTTIClassDescriptor*)pcurfield->getType(), srcdatasize))
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
		}

		return true;
	}

	return false;
}
