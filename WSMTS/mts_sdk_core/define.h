#ifndef DEFINE_H_
#define DEFINE_H_

#ifdef WIN32
#define LOCAL_SOCKETPAIR_AF AF_INET
#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <netdb.h>
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/utsname.h>
#define LOCAL_SOCKETPAIR_AF AF_UNIX
#define Sleep(x) usleep((x) * 1000);
#ifdef __ANDROID__
#include <pthread.h>
#endif
#endif

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"

#include "socket.h"
#include "event2/util.h"
#include "mts_lib.h"
#include "global.h"

const UINT      NAME_LEN = 128;
const UINT      AUTH_LEN = 1024;
const UINT      DOMAIN_LEN = 254;

const UINT      IP_LEN = 32;
const UINT      DNS_LEN = 128;
const int       MAX_SIZE_BUF = 16 * 1024;
const USHORT    CONST_VESION = 2;
const int       HEARTBEAT_TIME = 6;//最低 5秒，最高120秒
#define STR_ROOM_SIZE 32
#define ROOM_NUM 100
#define MAX_DOMAIN_PARSE 5  //域名解析最大失败次数
#define CONNECT_TIME_OUT 5  //链接超时时间
#define CLEAR_ROOM_INTERVAL 3 //定时删除房间
#define unused(x) ((void)(x))

typedef struct self_tv {
    struct event* ev_timeout;
    struct timeval tv;
} self_tv_t;

typedef struct room_event_mgr {
    struct bufferevent*		bev;							// socket 缓存
    self_tv_t				ev_heartbeat;					//心跳
    self_tv_t				ev_reconn;						//重新连接
} room_event_mgr_t;

typedef struct base_event {
    struct event_base *evbase;
    self_tv_t           delete_room_ev; //删除房间的事件
} base_event_t;

enum ENTER_ROOM_STATUS {
    ENTER_OK = 0,
    ENTER_DNS_ERROR = 1,		//1 获取房间网络地址失败
    ENTER_SOCKET_ERROR = 2,		//socket链接失败
    ENTER_AUTH_ERROR = 3,		//鉴权失败
    DNS_PARSE_ERROR = 4,        //域名解析失败
    ROOM_ID_ERROR = 5,             //错误的房间号
    ENTER_OTHER_ERROR = 10,		//其他错误
};

enum REQUEST_TYPE {
    REQUEST_LOG = 0,			//日志提交
    REQUEST_AUTH = 1,			//调度信息
};

enum REQUEST_CORE {
    CORE_OK					= 200,			//
    CORE_DEFAULT			= 10000,		//默认 指定值
    CORE_HOST_ERR			= 10001,		//url 解析有问题
    CORE_EXE_URL			= 10002,		//链接情况回调，链接对象和url属性为空
    CORE_MAKE_REQUEST		= 10003,		//evhttp_make_request 标记失败
    CORE_ROOM_LEAVE			= 10004,		//已经离开房间，这种错误无须打印日志
    CORE_REQUEST_CB_ERR		= 10005,		//请求结果回调获取返回数据异常
    CORE_AUTH_PARSE_FAIL	= 10006,		//解析鉴权、调度数据解析失败
    CORE_DNS_PARSE			= 10007,		//下发的ip或是域名解析失败
};

typedef struct cmd_msg {
    int room_id;
    int size;
    char msgbody[0];
} cmd_msg_t;

#endif