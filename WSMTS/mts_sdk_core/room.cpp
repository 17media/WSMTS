#include "stdafx.h"
#include "room.h"
#include "event2/bufferevent.h"
#include "clientdata.h"
#include "cmsg.h"
#include "event_mgr.h"
#include "event2/event.h"
#include "common.h"

CRoom::CRoom(int id)
{
    brecon = true;
    recon_times = 0;
    room_id = id;
    port = 0;
    fd = 0;
    memset(&ev_mgr, 0, sizeof(ev_mgr));
    memset(m_ip, 0, sizeof(m_ip));
    dodelete = false;
    login_status = false;
}

CRoom::~CRoom(void)
{
}

bool
CRoom::is_socketing()
{
    if (ev_mgr.bev == NULL)	{
        return false;
    }

    return true;
}

void
CRoom::close_socket()
{
    if (ev_mgr.bev) {
        int fd = bufferevent_getfd(ev_mgr.bev);
        bufferevent_setfd(ev_mgr.bev, -1);
        if ( fd > 0 ) {
            evutil_closesocket(fd);
            CClientData::Instance()->discon_call(this->room_id);
        }
        bufferevent_free(ev_mgr.bev);
        ev_mgr.bev = NULL;
    }
}

void
CRoom::leave_room()
{
    this->close_socket();
    login_status = false;
    brecon = true;
    recon_times = 0;
    stop_keeplive();
}

void
CRoom::destroy_room()
{
    leave_room();
    release_event();
    deleteroom();
}

void
CRoom::room_register()
{
    cmsg::Instance()->pro_register_new(this->room_id, CClientData::Instance()->get_appid(), CClientData::Instance()->get_authinfo());
    if (0 != bufferevent_write(this->ev_mgr.bev, cmsg::Instance()->get_buf(), cmsg::Instance()->get_size())) {
        CClientData::Instance()->enter_room_call(this->room_id, ENTER_AUTH_ERROR);        //鉴权失败
    }
}

void
CRoom::create_connect()
{
    if (ev_mgr.ev_reconn.ev_timeout) {
        event_free(ev_mgr.ev_reconn.ev_timeout);
        ev_mgr.ev_reconn.ev_timeout = NULL;
    }
    const char *ip = CClientData::Instance()->getip(this->recon_times);
    int port = CClientData::Instance()->getport();
    if (ip == NULL || port < 1) {
        return;
    }
    strcopy(this->m_ip, ip, sizeof(this->m_ip));
    this->port = port;
    if (tcp_connect_server(this, this->m_ip, this->port) < 1) {
        add_reconn_event(this, calc_delay(recon_times));
    }
}

void
CRoom::release_event()
{
    if (ev_mgr.ev_heartbeat.ev_timeout) {
        event_free(ev_mgr.ev_heartbeat.ev_timeout);
        ev_mgr.ev_heartbeat.ev_timeout = NULL;
    }

    if (ev_mgr.ev_reconn.ev_timeout) {
        event_free(ev_mgr.ev_reconn.ev_timeout);
        ev_mgr.ev_reconn.ev_timeout = NULL;
    }
}

void
CRoom::start_keeplive()
{
    this->stop_keeplive();
    base_event_t* ev_base = CClientData::Instance()->get_base_event();
    ev_mgr.ev_heartbeat.tv.tv_sec = 60;
    ev_mgr.ev_heartbeat.tv.tv_usec = 0;

    ev_mgr.ev_heartbeat.ev_timeout = evtimer_new(ev_base->evbase, heartbeat, this);
    evtimer_add(ev_mgr.ev_heartbeat.ev_timeout, &ev_mgr.ev_heartbeat.tv);
}

void
CRoom::stop_keeplive()
{
    if (ev_mgr.ev_heartbeat.ev_timeout) {
        event_free(ev_mgr.ev_heartbeat.ev_timeout);
        ev_mgr.ev_heartbeat.ev_timeout = NULL;
    }
}

