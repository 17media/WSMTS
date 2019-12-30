#include "stdafx.h"
#include "clientdata.h"
#include <event2/event.h>
#include "cmsg.h"
#include "event_mgr.h"
#include "event2/bufferevent.h"
#include "cjson.h"
#include "room.h"

CClientData* CClientData::obj_client = NULL;
CClientData::CClientData(void)
{
    memset(name, 0, NAME_LEN);
    memset(authority, 0, AUTH_LEN);
    memset(domain, 0, DOMAIN_LEN);
    memset(&base_event, 0, sizeof(base_event));
    memset(&ips, 0, sizeof(ips));
    memset(array_room, 0, sizeof(array_room));
    con_callback = NULL;
    discon_callback = NULL;
    enter_room_callback = NULL;
    m_barrage_cb = NULL;
    m_chat_cb = NULL;
    domain_parse_fail = 0;
    port = 0;
    appid = 0;
}

CClientData::~CClientData(void)
{
}

CClientData*
CClientData::Instance()
{
    if (NULL == obj_client) {
        obj_client = new CClientData();
    }

    return obj_client;
}

void
CClientData::set_cb(
    Con_Ballback con_callback,
    Discon_Callback discon_callback,
    Enter_Room_Callback enter_room_callback,
    Barrage_Callback barrage_cb,
    Chat_Callback chat_cb)
{
    this->con_callback = con_callback;
    this->discon_callback = discon_callback;
    this->enter_room_callback = enter_room_callback;
    this->m_barrage_cb = barrage_cb;
    this->m_chat_cb = chat_cb;
}

void
CClientData::connect_success_call(CRoom *roomobj)
{
    if (con_callback) {
        con_callback(roomobj->get_roomid(), true);
    }

    roomobj->room_register();
    //if(client.ev_heartbeat.timeout){
    //	event_free(client.ev_heartbeat.timeout);
    //	client.ev_heartbeat.timeout = NULL;
    //}

    //client.ev_heartbeat.tv.tv_sec = 5;
    //client.ev_heartbeat.tv.tv_usec = 0;
    //
    //client.ev_heartbeat.timeout = evtimer_new(client.base, heartbeat,NULL);
    //evtimer_add(client.ev_heartbeat.timeout, &client.ev_heartbeat.tv);

}

void
CClientData::discon_call(int room_id)
{
    if (this->discon_callback) {
        this->discon_callback(room_id);
    }
}

void
CClientData::enter_room_call(int room_id, ENTER_ROOM_STATUS code)
{
    if (this->enter_room_callback) {
        this->enter_room_callback(room_id, code);
    }

    if (code != ENTER_OK) {
        //EnterRoomFailToSend(code);
    }
}

void
CClientData::rev_data_call(const char* szData, int size)
{

}

void
CClientData::rev_barrage(int room_id, const char* szData, int size)
{
    if (m_barrage_cb) {
        m_barrage_cb(room_id, szData, size);
    }
}

void
CClientData::rev_private_chat(int room_id, const char* szData)
{
    if (!m_chat_cb) {
        return;
    }

    cJSON *recv_msg = NULL, *send_cid_item = NULL, *content = NULL;
    recv_msg = cJSON_Parse(szData);
    if (!recv_msg) {
        goto memory_release;
    }

    send_cid_item = cJSON_GetObjectItem(recv_msg, "send_cid");
    content = cJSON_GetObjectItem(recv_msg, "content");
    if (!send_cid_item || !content) {
        goto memory_release;
    }

    if (send_cid_item->type != cJSON_String || content->type != cJSON_String) {
        goto memory_release;
    }

    m_chat_cb(room_id, send_cid_item->valuestring, content->valuestring, strlen(content->valuestring));
memory_release:
    if (recv_msg) {
        cJSON_Delete(recv_msg);
    }
}

void
CClientData::set_base_info(const char* gdm, int port, int appid)
{
    strcopy(this->domain, gdm, DOMAIN_LEN);
    this->appid = appid;
    this->port = port;
}

const char*
CClientData::getip(int index /*= -1*/)
{
    if (index < 0) {
        index = 0;
    }
    return container_get_ip(&ips, index);
}

bool
CClientData::addip(const char* strDns, bool is_clear_old/*= false*/)
{
    if (NULL == strDns) {
        return false;
    }

    if (is_clear_old) {
        container_clear(&ips);
    }

    return get_ip_by_dns(strDns, &ips);
}

CRoom*
CClientData::getroombyid(int room_id)
{
    CRoom* roomobj = NULL;
    for (int i = 0; i < ROOM_NUM; i++) {
        roomobj = array_room[i];
        if (roomobj && !roomobj->get_is_del() && roomobj->get_roomid() == room_id) {
            return roomobj;
        }
    }
    return roomobj;
}

bool
CClientData::add_room(CRoom* roomobj)
{
    for (int i = 0; i < ROOM_NUM; i++) {
        if (array_room[i] == NULL) {
            array_room[i] = roomobj;
            return true;
        }
    }
    return false;
}

//room_id < 1是表示离开所有房间
void
CClientData::leave_room_byid(int room_id)
{
    if (room_id > 0) {
        CRoom* roomobj = this->getroombyid(room_id);
        if (roomobj) {
            roomobj->destroy_room();
        }
    } else { //离开所有房间
        CRoom* roomobj = NULL;
        for (int i = 0; i < ROOM_NUM; i++) {
            roomobj = this->array_room[i];
            if (roomobj && !roomobj->get_is_del()) {
                roomobj->destroy_room();
            }
        }
    }
}

void
CClientData::do_delete_room()
{
    CRoom* roomobj = NULL;
    for (int i = 0; i < ROOM_NUM; i++) {
        roomobj = array_room[i];
        if (roomobj != NULL && roomobj->get_is_del()) {
            array_room[i] = NULL;
            delete roomobj;
            roomobj = NULL;
        }
    }
}

