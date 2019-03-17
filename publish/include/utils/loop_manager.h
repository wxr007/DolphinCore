#ifndef DEF_LOOPMANAGER_H
#define DEF_LOOPMANAGER_H

#include <vector>
#include <list>
#include "utils/function.h"

using QCOOL::function;
using std::list;
using std::vector;

typedef QCOOL::function<void ()> RegFunc;

struct FuncInfo
{
	RegFunc call_;
	//other members;
};

class LoopFuncManager
{
public :
	LoopFuncManager(){}
	~LoopFuncManager(){}
	void RegistOnceCall(RegFunc& c)
	{
		FuncInfo tmp;
		tmp.call_ = c;
        one_calls_.push_back(tmp);
	}
	void RegistRepeatCall(RegFunc&c)
	{
        FuncInfo tmp;
        tmp.call_ = c;
		repeat_calls_.push_back(tmp);
	}
	//void UnRegist();

	void Run()
	{
		OncesCall();
		RepeatCall();
	}
private :
	void OncesCall()
	{
		list<FuncInfo>::iterator pos = one_calls_.begin();
		list<FuncInfo>::iterator end = one_calls_.end();
		while (pos != end)
		{
			pos->call_();
			++pos;
		}
		one_calls_.clear();
	}
	void RepeatCall()
	{
		vector<FuncInfo>::iterator pos = repeat_calls_.begin();
		vector<FuncInfo>::iterator end = repeat_calls_.end();
		while (pos != end)
		{
			pos->call_();
			++pos;
		}
	}
private :
	list<FuncInfo> one_calls_;
	vector<FuncInfo> repeat_calls_;
};

#endif