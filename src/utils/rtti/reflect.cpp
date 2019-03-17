#include"utils/rtti/type.h"


RTTIRepository* RTTIRepository::theRepository = NULL;

struct stAutoinitRTTIRepository
{
	stAutoinitRTTIRepository()
	{
		RTTIRepository::getInstance();
	}
	~stAutoinitRTTIRepository()
	{
		RTTIRepository::delInstance();
	}
};

stAutoinitRTTIRepository	AutoinitRTTIRepository;

inline unsigned hashFunction(const char* name)
{
	unsigned h = 0;

	while(*name != '\0')
	{
		h = ((h << 8) ^(*name++ & 0xFF)) | (h >> 24);
	}

	return h;
}

bool RTTIRepository::addClass(RTTIClassDescriptor* cls)
{
	AILOCKT(*this);
	unsigned hname = hashFunction(cls->name);

	for(RTTIClassDescriptor* cp = hashTable[hname % RTTI_CLASS_HASH_SIZE]; cp != NULL; cp = cp->collisionChain)
	{
		if(cp->hashCode == hname && strcmp(cp->name, cls->name) == 0)
		{
			return false;
		}
	}

	unsigned haliasname = hashFunction(cls->aliasname);

	for(RTTIClassDescriptor* cp = hashAliasTable[haliasname % RTTI_CLASS_HASH_SIZE]; cp != NULL; cp = cp->collisionAliasChain)
	{
		if(cp->hashAliasCode == haliasname && strcmp(cp->aliasname, cls->aliasname) == 0)
		{
			return false;
		}
	}

	cls->next = classes;
	classes = cls;
	cls->collisionChain = hashTable[hname % RTTI_CLASS_HASH_SIZE];
	hashTable[hname % RTTI_CLASS_HASH_SIZE] = cls;
	cls->hashCode = hname;
	cls->collisionAliasChain = hashAliasTable[haliasname % RTTI_CLASS_HASH_SIZE];
	hashAliasTable[haliasname % RTTI_CLASS_HASH_SIZE] = cls;
	cls->hashAliasCode = haliasname;
	return true;
}

RTTIClassDescriptor* RTTIRepository::findClassByAliasName(char const* pAliasName, bool bocasestr)
{
	unsigned h = hashFunction(pAliasName);
	AILOCKT(*this);

	for(RTTIClassDescriptor* cls = hashAliasTable[h % RTTI_CLASS_HASH_SIZE]; cls != NULL; cls = cls->collisionAliasChain)
	{
		if(cls->hashAliasCode == h && (strcmp(cls->aliasname, pAliasName) == 0 || (bocasestr && stricmp_q(cls->aliasname, pAliasName) == 0)))
		{
			return cls;
		}
	}

	return NULL;
}

RTTIClassDescriptor* RTTIRepository::findClass(char const* pclassname, bool bocasestr)
{
	bool bospace = false;
	char* name = (char*)strrchr(pclassname, 0x20);

	if(name == NULL)
	{
		name = (char*)pclassname;
	}
	else
	{
		bospace = true;
		name++;
	}

	unsigned h = hashFunction(name);
	AILOCKT(*this);

	for(RTTIClassDescriptor* cls = hashTable[h % RTTI_CLASS_HASH_SIZE]; cls != NULL; cls = cls->collisionChain)
	{
		if(cls->hashCode == h && (strcmp(cls->name, name) == 0 || (bocasestr && stricmp_q(cls->name, name) == 0)))
		{
			return cls;
		}
	}

	name = (char*)strrchr(pclassname, ':');

	if(name)
	{
		name++;
		unsigned h = hashFunction(name);

		for(RTTIClassDescriptor* cls = hashTable[h % RTTI_CLASS_HASH_SIZE]; cls != NULL; cls = cls->collisionChain)
		{
			if(cls->hashCode == h && (strcmp(cls->name, name) == 0 || (bocasestr && stricmp_q(cls->name, name) == 0)))
			{
				return cls;
			}
		}
	}
	else if(bospace)
	{
		name = (char*)pclassname;
		unsigned h = hashFunction(name);

		for(RTTIClassDescriptor* cls = hashTable[h % RTTI_CLASS_HASH_SIZE]; cls != NULL; cls = cls->collisionChain)
		{
			if(cls->hashCode == h && (strcmp(cls->name, name) == 0 || (bocasestr && stricmp_q(cls->name, name) == 0)))
			{
				return cls;
			}
		}
	}

	return NULL;
}

bool RTTIRepository::load(char const* filePath)
{
	return false;
}





