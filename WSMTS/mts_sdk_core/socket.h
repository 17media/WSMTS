/*CopyRight@网宿科技*/
#ifndef SOCKET_H_
#define  SOCKET_H_
#include "define.h"

class CRoom;
extern int tcp_connect_server(CRoom* room_obj, const char* server_ip, int port);
extern int creat_socket_fd();
extern void server_msg_cb(struct bufferevent* bev, void* arg);
//extern bool socket_send_msg(const char* szbuf,int size);
extern void event_cb(struct bufferevent *bev, short what, void *arg);

//连接服务器结果
extern void socket_connect_fail(short code);

typedef struct sockaddr SA;

#endif