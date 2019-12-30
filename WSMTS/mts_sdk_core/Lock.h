#ifndef _Lock_H
#define _Lock_H

#include "define.h"

//锁接口类
class IMyLock
{
public:
    virtual
    ~IMyLock() {}

    virtual void Lock() const = 0;
    virtual void Unlock() const = 0;
};


class CMutex: public IMyLock
{
public:
    CMutex(void);
    virtual ~CMutex(void);

    virtual void Lock() const;
    virtual void Unlock() const;

private:
#ifdef WIN32
    HANDLE m_mutex;
#else
    mutable pthread_mutex_t m_mutex;
#endif
};

//锁
class CLock
{
public:
    CLock(const IMyLock& m);
    virtual ~CLock();
private:
    const IMyLock& m_lock;
};


#endif