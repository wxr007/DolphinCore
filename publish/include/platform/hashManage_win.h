/*------------- HashManage.h
*
* Copyright (C): www.7cool.cn (2010)
* Author       : 
* Version      : V1.01
* Date         : 2010/11/25 18:02:28
*
*/ 
/*************************************************************
*hash
*************************************************************/
#ifndef _HASH_MANAGE_WIN_H__
#define _HASH_MANAGE_WIN_H__

#include <hash_map>
#include "utils/string/stringex.h"
#include "define/noncopyable.h"
#include "thread/lockObj.h"
/*************************************************************/

#ifdef _NOT_USE_STLPORT
using namespace stdext;
#else
using namespace std;
#endif

#ifdef _NOT_USE_STLPORT
#if (_MSC_VER == 1600)
	template < class keyT, class valueT, class _HashFcn = std::hash<keyT>, class _EqualKey = equal_to<keyT> >
#else
	template < class keyT, class valueT, class _HashFcn = stdext::hash<keyT>, class _EqualKey = equal_to<keyT> >
#endif
#else
template < class keyT, class valueT, class _HashFcn = hash<keyT>, class _EqualKey = equal_to<keyT> >
#endif
/*************************************************************
* ˵�� : 
*************************************************************/
class LimitHash: private zNoncopyable
{
public:
#ifdef _NOT_USE_STLPORT
	typedef stdext::hash_map< keyT, valueT>						hashmap;
#else
	typedef std::hash_map< keyT, valueT, _HashFcn, _EqualKey >	hashmap;
#endif

	typedef typename hashmap::iterator			iter;
	typedef iter								iterator;

	typedef typename hashmap::const_iterator	const_iter;
	typedef const_iter							const_iterator;

	typedef keyT								key;
	typedef valueT								value;

protected:
	hashmap										ets;

public:
	inline bool push(const keyT &key, valueT &value)
	{
		valueT tmp;

		if(!find(key, tmp))
		{
			return insert(key, value);
		}

		return false;
	}

	inline bool insert(const keyT &key, valueT &value)
	{
		ets[key] = value;
		return true;
	}

	inline bool find(const keyT &key, valueT &value) const
	{
		const_iter it = ets.find(key);

		if(it != ets.end())
		{
			value = it->second;
			return true;
		}
		else
		{
			return false;
		}
	}
	inline bool front(valueT &value) const
	{
		if(!ets.empty())
		{
			value = ets.begin()->second;
			return true;
		}

		return false;
	}
	LimitHash()
	{
	}
	~LimitHash()
	{
		clear();
	}
	inline void remove(const keyT &key, valueT &value)
	{
		iter it = ets.find(key);

		if(it != ets.end())
		{
			valueT findvalue = it->second;

			if(value == findvalue)
			{
				ets.erase(it);
			}
		}
	}
	inline void remove(const keyT &key)
	{
		ets.erase(key);
	}
	inline void clear()
	{
		ets.clear();
	}
	inline unsigned int size() const
	{
		return ets.size();
	}
	inline bool empty() const
	{
		return ets.empty();
	}
	inline hashmap& getmap()
	{
		return ets;
	}
	iter begin()
	{
		return ets.begin();
	}
	iter end()
	{
		return ets.end();
	}
	static __inline keyT mhkey(valueT &e)
	{
		static keyT tmp;
		return tmp;
	}
};

template < class valueT >
class LimitStrHash: public LimitHash<std::string, valueT, string_key_hash, string_key_equal>
{
};

template < class valueT >
class LimitStrCaseHash: public LimitHash<std::string, valueT, string_key_case_hash, string_key_case_equal>
{
};

template < class valueT >
class LimitPcharHash: public LimitHash<const char* , valueT, pchar_key_hash, pchar_key_equal>
{
};

template < class valueT >
class LimitPcharCaseHash: public LimitHash<const char* , valueT, pchar_key_case_hash, pchar_key_case_equal>
{
};


/*************************************************************
* ˵�� : 
*************************************************************/
template < class keyT, class valueT, class _HashFcn = hash<keyT>, class _EqualKey = equal_to<keyT> >
class MultiHash: private zNoncopyable
{
public:

#ifdef _NOT_USE_STLPORT
	typedef stdext::hash_multimap< keyT, valueT> mhashmap;
#else
	typedef std::hash_multimap< keyT, valueT, _HashFcn, _EqualKey > mhashmap;
#endif
	typedef typename mhashmap::iterator iter;
	typedef iter iterator;

	typedef typename mhashmap::const_iterator const_iter;
	typedef const_iter	const_iterator;

	typedef keyT	key;
	typedef valueT  value;
	typedef std::pair<const_iter, const_iter>	const_eiters;
	typedef std::pair<iter, iter>	eiters;
protected:
	mhashmap ets;
public:
	inline bool push(const keyT &key, valueT &value)
	{
		return insert(key, value);
	}
	inline bool insert(const keyT &key, valueT &value)
	{
		ets.insert(typename mhashmap::value_type(key, value));
		return true;
	}
	inline bool isfind(const keyT &key, valueT &value) const
	{
		eiters its = ets.equal_range(key);

		for(iter it = its.first; it != its.second; it++)
		{
			if(it->second == value)
			{
				return true;
			}
		}

		return false;
	}
	inline eiters equal_range(const keyT &key) const
	{
		return ets.equal_range(key);
	}
	inline bool find(const keyT &key, valueT &value, int nidx = 0, bool borandom = false) const
	{
		int rd = ets.count(key);

		if(rd > 0)
		{
			const_eiters its = ets.equal_range(key);

			if(its.first == its.second)
			{
				return false;
			}
			else
			{
				if(nidx < 0 || nidx >= rd)
				{
					if(borandom)
					{
						nidx = Random_Q(0, rd);
					}
					else
					{
						nidx = 0;
					}
				}

				for(const_iter it = its.first; it != its.second; it++)
				{
					if(nidx == 0)
					{
						value = it->second;
						return true;
					}

					nidx--;
				}

				value = its.first->second;
				return true;
			}
		}

		return false;
	}
	inline bool front(valueT &value) const
	{
		if(!ets.empty())
		{
			value = ets.begin()->second;
			return true;
		}

		return false;
	}
	MultiHash()
	{
	}
	~MultiHash()
	{
		clear();
	}
	inline void remove(const keyT &key, valueT &value)
	{
		eiters its = ets.equal_range(key);

		for(iter it = its.first; it != its.second; it++)
		{
			if(it->second == value)
			{
				ets.erase(it);
				break;
			}
		}

		return;
	}
// 	inline void remove(const keyT &key){
// 		ets.erase(key);
// 	}
	inline void clear()
	{
		ets.clear();
	}
	inline unsigned int size() const
	{
		return ets.size();
	}
	inline bool empty() const
	{
		return ets.empty();
	}
	inline mhashmap& getmap()
	{
		return ets;
	}
	iter begin()
	{
		return ets.begin();
	}
	iter end()
	{
		return ets.end();
	}
	static __inline keyT mhkey(valueT &e)
	{
		static keyT tmp;
		return tmp;
	}
};

template < class valueT >
class MultiStrHash: public MultiHash<std::string, valueT, string_key_hash, string_key_equal>
{
};

template < class valueT >
class MultiStrCaseHash: public MultiHash<std::string, valueT, string_key_case_hash, string_key_case_equal>
{
};

template < class valueT >
class MultiPcharHash: public MultiHash<const char* , valueT, pchar_key_hash, pchar_key_equal>
{
};

template < class valueT >
class MultiPcharCaseHash: public MultiHash<const char* , valueT, pchar_key_case_hash, pchar_key_case_equal>
{
};


/*************************************************************
* ˵�� : 
*************************************************************/
template < class valueT, class e1, class lockT = CIntLock >
class zHashManagerBase: public lockT
{
protected:
	e1 m_e1;
public:
	typedef typename e1::iter iter;
	typedef iter iterator;

	typedef typename e1::const_iter const_iter;
	typedef const_iter	const_iterator;

	typedef typename e1::key	key;
	typedef typename e1::value	value;
public:
	zHashManagerBase() {};
	virtual void removeValue(valueT& e) = 0;

	struct execValue
	{
		virtual bool exec(valueT& value) = 0;
		virtual ~execValue() {}
	};

	struct removeValue_Pred_Base
	{
		std::vector< valueT >& removed;
		removeValue_Pred_Base(std::vector< valueT >& pv): removed(pv) {}
		virtual ~removeValue_Pred_Base() {}

		virtual bool isIt(valueT& value) = 0;
		virtual void afterremove(valueT& value)
		{
			return;
		}
	};

	struct removeValue_Pred: public removeValue_Pred_Base
	{
	protected:
		std::vector< valueT > m_removed;
	public:
		removeValue_Pred(): removeValue_Pred_Base(m_removed)
		{
		}
	};

	inline int size()
	{
		AILOCKT(*this);
		return m_e1.size();
	}
	inline bool empty()
	{
		AILOCKT(*this);
		return m_e1.empty();
	}


	inline bool execEveryValue(execValue &eee)
	{
		AILOCKT(*this);
		iter it;

		for(it = m_e1.getmap().begin(); it != m_e1.getmap().end(); it++)
		{
			if(!eee.exec(it->second))
				return false;
		}

		return true;
	}

	inline void removeValue_if(removeValue_Pred_Base &pred)
	{
		INFOLOCK(*this);
		iter it = m_e1.getmap().begin();

		while(it != m_e1.getmap().end())
		{
			if(pred.isIt(it->second))
			{
				pred.removed.push_back(it->second);
			}

			it++;
		}

		for(unsigned int i = 0; i < pred.removed.size(); i++)
		{
			removeValue(pred.removed[i]);
			pred.afterremove(pred.removed[i]);
		}

		UNINFOLOCK(*this);
	}
	iter begin()
	{
		return m_e1.getmap().begin();
	}
	iter end()
	{
		return m_e1.getmap().end();
	}
};


/*************************************************************
* ˵�� : 
*************************************************************/
template < int i, class valueT, class keyT = int >
class zHashNone
{
public:
	inline bool push(const keyT &key, valueT &value)
	{
		return true;
	};
	inline bool insert(const keyT &key, valueT &value)
	{
		return true;
	};
	inline void remove(const keyT &key) {};
	inline void remove(const keyT &key, valueT &value) {};
	inline void clear() {};
	static __inline keyT mhkey(valueT &e)
	{
		static keyT tmp;
		return tmp;
	}
public:
	typedef int	hashmap;

	typedef int	iter;
	typedef iter iterator;

	typedef int	const_iter;
	typedef const_iter	const_iterator;

	typedef keyT	key;
	typedef valueT	value;
};

/*************************************************************
* ˵�� : 
*************************************************************/
template < class valueT, class e1,
         class e2 = zHashNone<2, valueT>,
         class e3 = zHashNone<3, valueT>,
         class lockT = CIntLock >
class zLHashManager3: public zHashManagerBase< valueT, e1, lockT >
{
protected:
	e2 m_e2;
	e3 m_e3;
public:
	inline bool addValue(valueT& e)
	{
		AILOCKT(*this);
		e1::key k1 = e1::mhkey(e);

		if(m_e1.push(k1, e))
		{
			e2::key k2 = e2::mhkey(e);

			if(m_e2.push(k2, e))
			{
				e3::key  k3 = e3::mhkey(e);

				if(m_e3.push(k3, e))
				{
					return true;
				}
				else
				{
					m_e2.remove(k2, e);
					m_e1.remove(k1, e);
				}
			}
			else
			{
				m_e1.remove(k1, e);
			}
		}

		return false;
	}

	inline bool insertValue(valueT& e)
	{
		AILOCKT(*this);
		e1::key k1 = e1::mhkey(e);

		if(m_e1.insert(k1, e))
		{
			e2::key k2 = e2::mhkey(e);

			if(m_e2.insert(k2, e))
			{
				e3::key  k3 = e3::mhkey(e);

				if(m_e3.insert(k3, e))
				{
					return true;
				}
				else
				{
					m_e2.remove(k2, e);
					m_e1.remove(k1, e);
				}
			}
			else
			{
				m_e1.remove(k1, e);
			}
		}

		return false;
	}

	virtual void removeValue(valueT& e)
	{
		AILOCKT(*this);
		e1::key k1 = e1::mhkey(e);
		e2::key k2 = e2::mhkey(e);
		e3::key k3 = e3::mhkey(e);
		m_e1.remove(k1, e);
		m_e2.remove(k2, e);
		m_e3.remove(k3, e);
	}
	zLHashManager3() {};
	~zLHashManager3() {};

	inline void clear()
	{
		AILOCKT(*this);
		m_e1.clear();
		m_e2.clear();
		m_e3.clear();
	}
};

/*************************************************************
* ˵�� : 
*************************************************************/
template < class valueT, class e1,
         class e2 = zHashNone<2, valueT>,
         class e3 = zHashNone<3, valueT>,
         class e4 = zHashNone<4, valueT>,
         class e5 = zHashNone<5, valueT>,
         class e6 = zHashNone<6, valueT>,
         class lockT = CIntLock >
class zLHashManager6: public zHashManagerBase< valueT, e1, lockT >
{
protected:
	e2 m_e2;
	e3 m_e3;
	e4 m_e4;
	e5 m_e5;
	e6 m_e6;
public:
	inline bool addValue(valueT& e)
	{
		AILOCKT(*this);
		e1::key k1 = e1::mhkey(e);

		if(m_e1.push(k1, e))
		{
			e2::key k2 = e2::mhkey(e);

			if(m_e2.push(k2, e))
			{
				e3::key  k3 = e3::mhkey(e);

				if(m_e3.push(k3, e))
				{
					e4::key  k4 = e4::mhkey(e);

					if(m_e4.push(k4, e))
					{
						e5::key  k5 = e5::mhkey(e);

						if(m_e5.push(k5, e))
						{
							e6::key  k6 = e6::mhkey(e);

							if(m_e6.push(k6, e))
							{
								return true;
							}
							else
							{
								m_e5.remove(k5, e);
								m_e4.remove(k4, e);
								m_e3.remove(k3, e);
								m_e2.remove(k2, e);
								m_e1.remove(k1, e);
							}
						}
						else
						{
							m_e4.remove(k4, e);
							m_e3.remove(k3, e);
							m_e2.remove(k2, e);
							m_e1.remove(k1, e);
						}
					}
					else
					{
						m_e3.remove(k3, e);
						m_e2.remove(k2, e);
						m_e1.remove(k1, e);
					}
				}
				else
				{
					m_e2.remove(k2, e);
					m_e1.remove(k1, e);
				}
			}
			else
			{
				m_e1.remove(k1, e);
			}
		}

		return false;
	}

	inline bool insertValue(valueT& e)
	{
		AILOCKT(*this);
		e1::key k1 = e1::mhkey(e);

		if(m_e1.insert(k1, e))
		{
			e2::key k2 = e2::mhkey(e);

			if(m_e2.insert(k2, e))
			{
				e3::key  k3 = e3::mhkey(e);

				if(m_e3.insert(k3, e))
				{
					e4::key  k4 = e4::mhkey(e);

					if(m_e4.insert(k4, e))
					{
						e5::key  k5 = e5::mhkey(e);

						if(m_e5.insert(k5, e))
						{
							e6::key  k6 = e6::mhkey(e);

							if(m_e6.insert(k6, e))
							{
								return true;
							}
							else
							{
								m_e5.remove(k5, e);
								m_e4.remove(k4, e);
								m_e3.remove(k3, e);
								m_e2.remove(k2, e);
								m_e1.remove(k1, e);
							}
						}
						else
						{
							m_e4.remove(k4, e);
							m_e3.remove(k3, e);
							m_e2.remove(k2, e);
							m_e1.remove(k1, e);
						}
					}
					else
					{
						m_e3.remove(k3, e);
						m_e2.remove(k2, e);
						m_e1.remove(k1, e);
					}
				}
				else
				{
					m_e2.remove(k2, e);
					m_e1.remove(k1, e);
				}
			}
			else
			{
				m_e1.remove(k1, e);
			}
		}

		return false;
	}

	virtual void removeValue(valueT& e)
	{
		AILOCKT(*this);
		e1::key k1 = e1::mhkey(e);
		e2::key k2 = e2::mhkey(e);
		e3::key  k3 = e3::mhkey(e);
		e4::key  k4 = e4::mhkey(e);
		e5::key  k5 = e5::mhkey(e);
		e6::key  k6 = e6::mhkey(e);
		m_e6.remove(k6, e);
		m_e5.remove(k5, e);
		m_e4.remove(k4, e);
		m_e3.remove(k3, e);
		m_e2.remove(k2, e);
		m_e1.remove(k1, e);
	}
	zLHashManager6() {};
	~zLHashManager6() {};

	inline void clear()
	{
		AILOCKT(*this);
		m_e1.clear();
		m_e2.clear();
		m_e3.clear();
		m_e4.clear();
		m_e5.clear();
		m_e6.clear();
	}
};

#endif //_HASH_MANAGE_WIN_H__

