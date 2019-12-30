#ifndef CROOM_H
#define CROOM_H
#include "define.h"

class CRoom
{
public:
    CRoom(int id);
    ~CRoom(void);
public:
    room_event_mgr_t*
    get_event_mgr()
    {
        return &ev_mgr;
    };
    int
    get_roomid()
    {
        return room_id;
    }
    int
    get_recon_times()
    {
        return recon_times;
    }
    bool
    get_login_status()
    {
        return login_status;
    }
    bool
    get_is_del()
    {
        return dodelete;
    }
    bool
    is_need_recon()
    {
        return brecon;
    }
public:
    void
    set_status(bool evalue)
    {
        this->login_status = evalue;
    }
    void
    deleteroom()
    {
        dodelete = true;
    }
    void
    set_recon(bool value)
    {
        this->brecon = value;
    }
    void
    add_redotimes()
    {
        recon_times++;
    }
    void
    set_room_id(int value)
    {
        this->room_id = value;
    }
public:
    bool is_socketing();
    void room_register();
    void create_connect();
    void close_socket();
    void release_event();
    void leave_room();
    void destroy_room();
    void start_keeplive();
    void stop_keeplive();
private:
    int     fd;
    char    m_ip[32];
    int     port;
    int     room_id;
    int     recon_times;    //重连次数 失败之后再试下一个ip，成功就index不改变
    bool    brecon;         //是否需要重连
    bool    dodelete;
    bool    login_status;

    room_event_mgr_t ev_mgr;
};


#endif