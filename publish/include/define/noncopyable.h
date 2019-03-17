#ifndef _NONCOPYABLE_H__
#define _NONCOPYABLE_H__

class zNoncopyable
{
protected://子类可继承			

    zNoncopyable() {};
    ~zNoncopyable() {};
private://子类不可继承
    zNoncopyable(const zNoncopyable&);//复制构造函数						
    const zNoncopyable & operator= (const zNoncopyable &);//重载赋值操作符

};
#endif //_NONCOPYABLE_H__
