
#ifndef _REDIS_ARG_H__
#define _REDIS_ARG_H__

#include <vector>
#include <string>
#include <stdint.h>

typedef const char * TRedisArg;
struct RedisCmdArg
{
	std::vector<TRedisArg> args;
	std::vector<size_t>    arglen;
	int32_t argc;
	RedisCmdArg():argc(0)
	{
		args.reserve(1024);
		arglen.reserve(1024);
	}
	void InitRedisCmdArg(int32_t argnum)
	{
		args.clear();
		arglen.clear();
		argc = argnum;
	}
	//return the pos;
	size_t AppendArg(const std::string& str)
	{
		args.push_back(str.c_str());
		arglen.push_back(str.size());
		return args.size();
	}
	size_t AppendArg(const char* str,size_t len)
	{
		args.push_back(str);
		arglen.push_back(len);
		return args.size();
	}

	size_t AppendArg(const std::vector<std::string>& vecstr)
	{
		size_t vecsz = vecstr.size();
		for (size_t i=0; i< vecsz;++i)
		{
			args.push_back(vecstr.at(i).c_str());
			arglen.push_back(vecstr.at(i).size());
		}
		return args.size();
	}
	const char **Args()
	{
		if (args.empty())
		{
			return NULL;
		}
		return (const char **)(&args[0]);
	}
	const size_t *Arglen()
	{
		return &(arglen[0]);
	}
};


#endif //_REDIS_ARG_H__

