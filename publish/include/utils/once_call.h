
#ifndef _ONCE_CALL_H__
#define _ONCE_CALL_H__

#ifdef _USE_CXX11_

#include <mutex>

namespace QCOOL
{
using std::call_once;
using std::once_flag;
}

#else//not def _USE_CXX11_;
//using boost;
#include <boost/thread/once.hpp>

namespace QCOOL
{
using boost::call_once;
using boost::once_flag;
}

#endif

namespace QCOOL
{
    template<typename T>
    class Singleton
    {
    public:
        static T& Instance()
        {
            call_once(run_flag_,Singleton::init);
            return  *m_value;
        }
		static T* instance()
		{
			call_once(run_flag_,Singleton::init);
			return  m_value;
		}
		static T& getMe()//zc:fixed 为了适配1.5的单例
		{
			call_once(run_flag_,Singleton::init);
			return  *m_value;
		}
    protected:
		Singleton(){}
		virtual ~Singleton(){}
        //实例化T类型对象
	private:
        static void init()
        {
            m_value = new T();
            ::atexit(destroy);
        }
        //删除T类型对象
        static void destroy()
        {
            typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
            T_must_be_complete_type dummy; (void) dummy;
            if (m_value)
            {
                delete m_value;
                m_value = NULL;
            }
        }
    private:
        static once_flag run_flag_;
        static T*             m_value;
    };

	template<typename T>
	once_flag Singleton<T>::run_flag_;

	template<typename T>
	T* Singleton<T>::m_value = NULL;
}

#endif //_ONCE_CALL_H__
