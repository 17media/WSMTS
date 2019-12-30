#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

#include "define.h"
#include "ipcontainer.h"
#include "common.h"
class CRoom;

class CClientData
{
public:
    CClientData(void);
    ~CClientData(void);

    void
    setname(const char *name)
    {
        strcopy(this->name, name, NAME_LEN);
    }
    void
    setauth(const char *auth)
    {
        strcopy(this->authority, auth, AUTH_LEN);
    }
    const char*
    get_domain()
    {
        return this->domain;
    }
    void
    setport(int port)
    {
        this->port = port;
    }
    int
    getport()
    {
        return this->port;
    }
    //是否连接成功
    void set_base_info(const char* gdm, int port, int appid);
    int
    get_appid()
    {
        return appid;
    }
    const char*
    get_authinfo()
    {
        return this->authority;
    }
    bool addip(const char* strDns, bool isclearold = false);
    const char* getip(int index = -1);
    bool add_room(CRoom* roomobj);
    base_event_t*
    get_base_event()
    {
        return &base_event;
    }
    CRoom* getroombyid(int room_id);
    void leave_room_byid(int room_id);
    pthread_t
    getThreadPid()
    {
        return this->tid;
    }
    void
    setThreadPid(pthread_t tid)
    {
        this->tid = tid;
    }
public:
    void set_cb(Con_Ballback con_callback, Discon_Callback discon_callback,
                Enter_Room_Callback enter_room_callback,
                Barrage_Callback barrage_cb,
                Chat_Callback chat_cb
               );
    void connect_success_call(CRoom *roomobj);
    void discon_call(int room_id);
    void enter_room_call(int room_id, ENTER_ROOM_STATUS code);
    void rev_data_call(const char* szData, int size);
    void rev_barrage(int room_id, const char* szData, int size);
    void rev_private_chat(int room_id, const char* szData);
    void do_delete_room();
public:
    static CClientData* Instance();
    int domain_parse_fail;
private:
    static CClientData * obj_client;
    base_event_t        base_event;
    char                name[NAME_LEN];
    char                domain[DOMAIN_LEN];
    char                authority[AUTH_LEN];
    int                 port;
    ipcontainer_t       ips;
    int                 appid;
    CRoom*              array_room[ROOM_NUM];// 暂定进入的最大房间数100
    pthread_t           tid;

    //当房间个数为0时
    Con_Ballback                con_callback;
    Discon_Callback             discon_callback;
    Enter_Room_Callback         enter_room_callback;
    Barrage_Callback            m_barrage_cb;
    Chat_Callback               m_chat_cb;
};


#endif