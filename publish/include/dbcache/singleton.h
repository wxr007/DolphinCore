#pragma  once

// #include <boost/noncopyable.hpp>
// #include <pthread.h>
// #include <stdlib.h> // atexit
// 
// namespace QCOOL
// {
// //单例模式，保证只有一个实例
// template<typename T>
// class Singleton : boost::noncopyable
// {
// protected:
// 	Singleton()
// 	{
// 
// 	}
// 	~Singleton()
// 	{
// 
// 	}
// public:
// 	//静态函数实例化T类型对象
// 	static __inline T& Instance()
// 	{
// 		//pthread_once可以保证其在多线程环境中只执行一次m_ponce表示十分实行过
// 		pthread_once(&m_ponce, &Singleton::init);
// 		return *m_value;
// 	}
// 	//获得类型实例
// 	static __inline T& GetInstance()
// 	{
// 		if (m_value)
// 		{
// 			return  *m_value;
// 		}
// 		else
// 		{
// 			return Instance();
// 		}
// 	}
// 
// 
// private:
// 	//实例化T类型对象
// 	static __inline void init()
// 	{
// 		m_value = new T();
// 		::atexit(destroy);
// 	}
// 	//删除T类型对象
// 	static __inline void destroy()
// 	{
// 		if (m_value)
// 		{
// 			delete m_value;
// 			m_value = NULL;
// 		}
// 	}
// private:
// 	static pthread_once_t m_ponce;
// 	static T*             m_value;
// };
// 
// template<typename T>
// pthread_once_t Singleton<T>::m_ponce = PTHREAD_ONCE_INIT;
// 
// template<typename T>
// T* Singleton<T>::m_value = NULL;
// 
// }