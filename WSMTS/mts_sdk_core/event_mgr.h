
#ifndef EVENT_MGR_H
#define EVENT_MGR_H
#include "define.h"
class CMutex;
class CRoom;
extern bool init_event();
extern void cmd_notify_cb(int fd, short type, void *arg);
extern void reconnect(int fd, short type, void *arg);
extern void heartbeat(int fd, short type, void *arg);
extern void http_host_ip(int fd, short type, void *arg);
extern void enter_room_cb(evutil_socket_t fd, short what, void *arg);
extern void send_barrage_cb(evutil_socket_t fd, short what, void *arg);
extern void send_private_chat_cb(evutil_socket_t fd, short what, void *arg);
extern void leave_room_cb(evutil_socket_t fd, short what, void *arg);
extern struct evbuffer *create_evbuffer(int room_id, const char* data, int size);


//清除所有事件
extern void release_notify_event();

//dealy 单位毫秒秒
extern void add_reconn_event(CRoom* roomobj, int delay);

//设置socket 创建连接超时时间 单位秒
extern void set_docon_timeout(int nsec);


#endif

