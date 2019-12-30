#ifndef CMSG_H_
#define CMSG_H_
#include "define.h"

enum MSG_TYPE_ENUM {
    MSG_SUB_ROOM = 10,         /* 废弃，新值17 */
    MSG_AUTH_ROOM_ACK = 11,    /* 二级MTS鉴权响应值 */
    MSG_HEARTBEAT = 12,
    MSG_BARRAGE = 13,          /* 废弃，新值19 */
    MSG_PRIVATE_CHAT = 14,
    MSG_AUTH_ROOM_REQ = 17,    /* 新前端鉴权 */
    MSG_AUTH_WITH_ROOM = 18, /* 支持无鉴权模式 */
    MSG_BARRAGE_PUBLIC = 19, /* 新的弹幕协议 在原来的基础上去掉 房间号，优先级 */
};
class CRoom;
#pragma pack (1)
struct  msg_head_s {
    USHORT	usVe;
    UCHAR	usOp;
    int		usSize;
};

struct room_register_ret_s {
    int		room_id;
    int		code;
};

struct public_struct_s {
    char data[0];
};

struct heartbeat_s {
    int room_id;
};

#pragma pack ()

class cmsg
{
public:
    cmsg(void);
    ~cmsg(void);
    static cmsg* Instance();

    const char*
    get_buf		(void) const
    {
        return szbuf;
    }
    int
    get_size	(void) const
    {
        return ntohl(msg_head.usSize) + (int)sizeof(msg_head_s);
    }
    bool			init(const char* szbuf, int size);
    void			process(CRoom* roomobj);
public:
    bool			pro_register_new(int idroom, int appid, const char* auth);
    bool            sub_room(const char* data, int size);
    bool			pro_barrage(const char* strbarrage, int size);
    bool            pro_chat_data(const char* data, int size);
    bool			pro_heartbeat();
    void
    set_content_size(int size)
    {
        msg_head.usSize = ntohl(size);
    }
    int
    get_content_size()
    {
        return htonl(msg_head.usSize);
    }
private:
    static cmsg*		obj_msg;

    //内存主体
    union {
        msg_head_s			msg_head;
        char				szbuf[MAX_SIZE_BUF];

    };

    union {
        void*							pinfo;
        room_register_ret_s*			register_ret;
        public_struct_s*                ps_data;
    };
};
#endif