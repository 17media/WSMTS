#include "stdafx.h"

#include "socket.h"
#include "event_mgr.h"
#include "event2/bufferevent.h"
#include "event2/event.h"
#include <errno.h>
#include "clientdata.h"
#include "cmsg.h"
#include "./event2/buffer.h"
#include "room.h"

int
tcp_connect_server(CRoom* room_obj, const char* server_ip, int port)
{
    //每次调用 tcp_connect_server 统计连接次数就+1
    room_obj->add_redotimes();
    if (server_ip == NULL) {
        return -1;
    }

    int sockfd, status;
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    status = inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
    if ( status == 0 ) {
        errno = EINVAL;
        return -1;
    }

    sockfd = creat_socket_fd();
    if ( sockfd == -1 ) {
        return sockfd;
    }

    room_event_mgr_t* ev_mgr = room_obj->get_event_mgr();
    base_event_t* base_ev = CClientData::Instance()->get_base_event();
    ev_mgr->bev = bufferevent_socket_new(base_ev->evbase, sockfd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
    if (ev_mgr->bev == NULL) {
        return -1;
    }

    evutil_make_socket_nonblocking(sockfd);
    bufferevent_setcb(ev_mgr->bev, server_msg_cb, NULL, event_cb, room_obj);
    bufferevent_enable(ev_mgr->bev, EV_READ | EV_PERSIST);

    struct timeval tv_read = {CONNECT_TIME_OUT, 0};
    bufferevent_set_timeouts(ev_mgr->bev, &tv_read, NULL);

    bufferevent_socket_connect(ev_mgr->bev, (SA*)&server_addr, sizeof(server_addr));
    return sockfd;
}

void
server_msg_cb(struct bufferevent* bev, void* arg)
{
    static char msg[MAX_SIZE_BUF];
    int nloop = 0; //防止死循环的方法
    int len = 0;
    static struct evbuffer*		buf = evbuffer_new();
    static cmsg ReadMsg;
    CRoom* roomobj = (CRoom*)arg;

    while (++nloop <= 2000) {
        bufferevent_read_buffer(bev, buf);
        len = evbuffer_copyout(buf, msg, sizeof(msg_head_s));
        if (len < (int)sizeof(msg_head_s)) {
            break;
        }

        ReadMsg.init(msg, sizeof(msg_head_s));
        if (ReadMsg.get_size() >= MAX_SIZE_BUF) {
            goto ERROR_GOTO;
        }

        len = evbuffer_copyout(buf, msg, ReadMsg.get_size());
        if (len < ReadMsg.get_size()) {
            break;
        } else if (len > ReadMsg.get_size()) {
            goto ERROR_GOTO;
        }

        if (evbuffer_drain(buf, len) < 0 ) {
            goto ERROR_GOTO;
        }

        ReadMsg.init(msg, len);
        ReadMsg.process(roomobj);
    }

    return;
ERROR_GOTO:
    evbuffer_drain(buf, evbuffer_get_length(buf));
    roomobj->close_socket();
    roomobj->destroy_room();
    return;
}

void
event_cb(struct bufferevent *bev, short what, void *arg)
{
    CRoom* roomobj = (CRoom*)arg;
    if (what & BEV_EVENT_CONNECTED) {
        //链接成功
        struct timeval tv_read = {259200, 0}; //把超时时间调整成很长的时间 3day，没有找到清除的方法
        bufferevent_set_timeouts(bev, &tv_read, NULL);
        CClientData::Instance()->connect_success_call(roomobj);
        return;
    }

    room_event_mgr_t* ev_mgr = roomobj->get_event_mgr();
    if (bev == ev_mgr->bev) {
        ev_mgr->bev = NULL;
    }
    bufferevent_free(bev);

    //打印日志 连接失败
    if (what & BEV_EVENT_TIMEOUT || what & BEV_EVENT_ERROR) {
        socket_connect_fail(what);
    }

    //当还没进入房间，无须通知重连
    if (roomobj->is_need_recon()) {
        add_reconn_event(roomobj, calc_delay(roomobj->get_recon_times()));
    }

    //如果还未登录成功过的，改函数无须调用
    if (roomobj->get_login_status()) {
        CClientData::Instance()->discon_call(roomobj->get_roomid());
        roomobj->set_status(false);
    }
}

int
creat_socket_fd()
{
    int sockfd = -1;
#ifdef WIN32
    WSADATA wsd;
    if ((sockfd = WSAStartup(MAKEWORD(2, 2), &wsd)) != 0) {
        sockfd = -1;
        return sockfd;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

#else
    sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
#endif

    return sockfd;
}

void
socket_connect_fail(short code)
{

}
