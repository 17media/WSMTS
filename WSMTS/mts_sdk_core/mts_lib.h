#ifndef __MTS_LIB_H__
#define __MTS_LIB_H__

#include "global.h"
typedef void (*Con_Ballback)(int room_id, bool bRet);
typedef void (*Discon_Callback)(int room_id);
typedef void (*Enter_Room_Callback)(int room_id, int err_code);
typedef void (*Barrage_Callback)(int room_id, const char* szbuf, int nsize);
typedef void (*Chat_Callback)(int room_id, const char* tg_cid, const char* szbuf, int nsize);


/*
* 初始化 sdk
* gdm 服务器ip或域名
* appid由网宿提供
*/
extern bool init(const char* gdm_server, int port, int appid);
/*
* con_cb				链接结果回调 btype=false表示失败，true表示成功
* discon_cb				链接断开回调 room_id房间号
* enter_room_callback	每次调用enter_room接口，sdk通过回调 enter_room_callback 返回进入房间的结果
* err_code=0成功,1		获取房间网络地址失败,2 socket链接失败,3 鉴权失败,10 其他错误
* barrage_cb			弹幕数据，send_barrage 中szbuf数据原封返回
* chat_cb				私聊回调, send_private_chat 中szbuf数据原封返回
*/
extern void set_callback(Con_Ballback con_cb,
                         Discon_Callback		discon_cb,
                         Enter_Room_Callback enter_room_cb,
                         Barrage_Callback	barrage_cb,
                         Chat_Callback		chat_cb
                        );

/*
* room_id 房间id
* authority 鉴权信息 可以不填填写
* 返回值：当参数非法返回false (注：进入房间结果在enter_room_callback返回)
*/
extern bool enter_room(int room_id, const char* authority);

/*
* szbuf 发送字节流数据，服务器只做转发，不解析
* size 字节流长度
*/
extern bool send_barrage(int room_id, const char* szbuf, int size);

/*
* 私聊
* tg_cid 对方的cid
* szbuf 发送字节流数据，服务器只做转发，不解析
* size 字节流长度
*/
extern bool send_private_chat(int room_id, const char *send_cid, const char *recv_cid, const char* szbuf, int size);

/*
* 离开房间
* room_id 是房间号
*/
extern bool leave_room(int room_id);

/*
退出程序
*/
extern void finish();

/*
 * 设置debug开关。
 * value true,表示开启调试信息；false,表示关闭
 */
extern void set_debug_on_off(bool value);

/*
 * 获取版本号信息，数据为字符串，不需要释放
 */
extern const char *get_mts_sdk_version();

#endif
