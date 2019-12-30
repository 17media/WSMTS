#include "stdafx.h"
#include "event_mgr.h"

#include "define.h"

#include "event2/thread.h"
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "event2/util.h"
#include "define.h"
#include "clientdata.h"
#include "cmsg.h"
#include "event2/buffer.h"
#include "Lock.h"
#include "room.h"

CMutex					g_Mutex;
int						g_docon_timeout = 5;

static bool parse_domain(int room_id);
static void do_enter_room(int room_id);
static void create_delete_room_ev(base_event_t* ev_mgr);

extern bool
init_event()
{
#ifdef WIN32
    evthread_use_windows_threads();
    WSADATA WSAData;
    WSAStartup(0x101, &WSAData);
#else
    evthread_use_pthreads();
#endif
    base_event_t* ev_mgr = CClientData::Instance()->get_base_event();
    ev_mgr->evbase = event_base_new();

    create_delete_room_ev(ev_mgr);
    return true;

}

static void
run_clear_delete_room(int fd, short type, void *arg)
{
    base_event_t* ev_mgr = CClientData::Instance()->get_base_event();
    event_add(ev_mgr->delete_room_ev.ev_timeout, &ev_mgr->delete_room_ev.tv);
    CClientData::Instance()->do_delete_room();
}

static void
create_delete_room_ev(base_event_t* ev_mgr)
{
    ev_mgr->delete_room_ev.tv.tv_sec = CLEAR_ROOM_INTERVAL;
    ev_mgr->delete_room_ev.tv.tv_usec = 0;

    ev_mgr->delete_room_ev.ev_timeout = evtimer_new(ev_mgr->evbase, run_clear_delete_room, NULL);
    evtimer_add(ev_mgr->delete_room_ev.ev_timeout, &ev_mgr->delete_room_ev.tv);
}

struct evbuffer *
create_evbuffer(int room_id, const char* data, int size)
{
    cmd_msg_t cmd_msg;
    cmd_msg.room_id = room_id;
    cmd_msg.size = size;

    struct evbuffer *evbuf = evbuffer_new();
    evbuffer_add(evbuf, &cmd_msg, sizeof(cmd_msg_t));
    evbuffer_add(evbuf, data, size);

    return evbuf;
}

void
add_reconn_event(CRoom* roomobj, int delay)
{
    delay += 1;
    room_event_mgr_t* ev_mgr = roomobj->get_event_mgr();
    base_event_t* ev_base = CClientData::Instance()->get_base_event();
    ev_mgr->ev_reconn.tv.tv_sec = delay / 1000;
    ev_mgr->ev_reconn.tv.tv_usec = delay % 1000;

    ev_mgr->ev_reconn.ev_timeout = evtimer_new(ev_base->evbase, reconnect, roomobj);
    evtimer_add(ev_mgr->ev_reconn.ev_timeout, &ev_mgr->ev_reconn.tv);
}

void
reconnect(int fd, short type, void *arg)
{
    CRoom* roomobj = (CRoom*)arg;
    if (!roomobj) {
        return;
    }

    room_event_mgr_t* ev_mgr = roomobj->get_event_mgr();
    if (ev_mgr->ev_reconn.ev_timeout) { //清除定时器
        event_free(ev_mgr->ev_reconn.ev_timeout);
        ev_mgr->ev_reconn.ev_timeout = NULL;
    }

    roomobj->close_socket();
    roomobj->create_connect();
}

extern void
release_notify_event()
{
    base_event_t* ev_base = CClientData::Instance()->get_base_event();

    if (ev_base->delete_room_ev.ev_timeout) {
        event_free(ev_base->delete_room_ev.ev_timeout);
        ev_base->delete_room_ev.ev_timeout = NULL;
    }
}

void
heartbeat(int fd, short type, void *arg)
{
    CRoom *roomobj = (CRoom*) arg;
    room_event_mgr_t *ev_mgr = roomobj->get_event_mgr();

    if (roomobj->get_login_status()) {
        cmsg::Instance()->pro_heartbeat();

        const char *sz_data = cmsg::Instance()->get_buf();
        int size = cmsg::Instance()->get_size();

        if (ev_mgr->bev) {
            bufferevent_write(ev_mgr->bev, sz_data, size);
        }

        if (ev_mgr->ev_heartbeat.ev_timeout) {
            event_add(ev_mgr->ev_heartbeat.ev_timeout, &ev_mgr->ev_heartbeat.tv);
        }
    }
}

static void
do_enter_room(int room_id)
{
    CRoom* room_obj = CClientData::Instance()->getroombyid(room_id);
    if (!room_obj) {
        room_obj = new CRoom(room_id);
        if (!CClientData::Instance()->add_room(room_obj)) {
            delete room_obj;
            return;
        }
    } else {
        room_obj->leave_room();
        room_obj->release_event();
    }

    add_reconn_event(room_obj, 0);
}

static bool
parse_domain(int room_id)
{
    bool ret = CClientData::Instance()->addip(CClientData::Instance()->get_domain(), true);
    debug("parse_domain ret %d\n", ret);
    if (!ret) {
        if (CClientData::Instance()->domain_parse_fail > MAX_DOMAIN_PARSE) {
            CClientData::Instance()->enter_room_call(room_id, DNS_PARSE_ERROR);
            return false;
        }
        CClientData::Instance()->domain_parse_fail++;
        base_event_t *evbase = CClientData::Instance()->get_base_event();
        event_base_once(evbase->evbase, -1, EV_TIMEOUT, enter_room_cb, (void *)room_id, NULL);
        return false;
    }
    return true;
}

void
enter_room_cb(evutil_socket_t fd, short what, void *arg)
{
    int room_id = *(int *)&arg;
    if (parse_domain(room_id)) {
        do_enter_room(room_id);
    }
}

void
send_barrage_cb(evutil_socket_t fd, short what, void *arg)
{
    struct evbuffer *evbuf = (struct evbuffer *)arg;
    size_t len = evbuffer_get_length(evbuf);
    cmd_msg_t *cmd_msg = (cmd_msg_t *)evbuffer_pullup(evbuf, len);

    CRoom *roomobj = CClientData::Instance()->getroombyid(cmd_msg->room_id);
    if (roomobj) {
        room_event_mgr_t *ev_mgr = roomobj->get_event_mgr();
        if (ev_mgr && ev_mgr->bev) {
            cmsg::Instance()->pro_barrage(cmd_msg->msgbody, cmd_msg->size);
            bufferevent_write(ev_mgr->bev, cmsg::Instance()->get_buf(), cmsg::Instance()->get_size());
        }
    }
    evbuffer_free(evbuf);
}

void
send_private_chat_cb(evutil_socket_t fd, short what, void *arg)
{
    struct evbuffer *evbuf = (struct evbuffer *)arg;
    size_t len = evbuffer_get_length(evbuf);
    cmd_msg_t *cmd_msg = (cmd_msg_t *)evbuffer_pullup(evbuf, len);

    CRoom *roomobj = CClientData::Instance()->getroombyid(cmd_msg->room_id);
    if (roomobj) {
        room_event_mgr_t *ev_mgr = roomobj->get_event_mgr();
        if (ev_mgr && ev_mgr->bev) {
            cmsg::Instance()->pro_chat_data(cmd_msg->msgbody, cmd_msg->size);
            bufferevent_write(ev_mgr->bev, cmsg::Instance()->get_buf(), cmsg::Instance()->get_size());
        }
    }
    evbuffer_free(evbuf);
}

void
leave_room_cb(evutil_socket_t fd, short what, void *arg)
{
    int room_id = *(int *)&arg;
    CClientData::Instance()->leave_room_byid(room_id);
}


