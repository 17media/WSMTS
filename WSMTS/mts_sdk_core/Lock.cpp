#include "stdafx.h"
#include "Lock.h"
#ifndef WIN32
#include <pthread.h>
#endif


//创建一个匿名互斥对象
CMutex::CMutex()
{
#ifdef WIN32
    m_mutex = ::CreateMutex(NULL, FALSE, NULL);
#else
    pthread_mutex_init(&m_mutex, NULL);
#endif // WIN32
}

//销毁互斥对象，释放资源
CMutex::~CMutex()
{
#ifdef WIN32
    ::CloseHandle(m_mutex);
#else
    pthread_mutex_destroy(&m_mutex);
#endif // WIN32
}

void
CMutex::Lock() const
{
#ifdef WIN32
    WaitForSingleObject(m_mutex, INFINITE);
#else
    pthread_mutex_lock(&m_mutex);
#endif // WIN32
}

void
CMutex::Unlock() const
{
#ifdef WIN32
    ::ReleaseMutex(m_mutex);
#else
    pthread_mutex_unlock(&m_mutex);
#endif // WIN32
}

CLock::CLock(const IMyLock& m) : m_lock(m)
{
    m_lock.Lock();
}

CLock::~CLock()
{
    m_lock.Unlock();
}
