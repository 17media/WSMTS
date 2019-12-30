#include "stdafx.h"

#include "thread.h"
#ifdef WIN32
#include <direct.h>
#include <process.h>
#else
#include <pthread.h>
#endif

#include "socket.h"
#include "define.h"
#include "event2/event.h"
#include "event2/thread.h"
#include "event2/bufferevent.h"
#include "clientdata.h"
#include "event_mgr.h"

extern volatile bool		stop_event;

#ifdef WIN32
void
thread_run(void* param)
#else
void* thread_run(void* param)                // 实现run函数
#endif
{
    base_event_t *ev_base = CClientData::Instance()->get_base_event();
    if (ev_base->evbase) {
        int nret = event_base_loop(ev_base->evbase, EVLOOP_NO_EXIT_ON_EMPTY);
        if (nret == -1) {
            debug("ERROR!!!!! event_base_loop \n");
        }
    }

    CClientData::Instance()->leave_room_byid(0);
    release_notify_event();
#ifndef WIN32
    return NULL;
#endif
}

bool
create_thread()
{
#ifdef WIN32
    if (-1 == _beginthread(thread_run, 0, NULL)) { // create thread ok
        return false;
    }
#else
    int err;
    pthread_t tid;
    err = pthread_create(&tid, NULL, thread_run, NULL);
    if (err != 0) {
        return false;
    }
    CClientData::Instance()->setThreadPid(tid);
#endif

    return true;
}
