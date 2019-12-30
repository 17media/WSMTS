#include "stdafx.h"
#include "cmsg.h"
#include "clientdata.h"
#include "event_mgr.h"
#include "cjson.h"
#include "room.h"

cmsg* cmsg::obj_msg = NULL;
cmsg::cmsg()
{
    memset(szbuf, 0, MAX_SIZE_BUF);
    pinfo = szbuf + sizeof(msg_head_s);
    msg_head.usVe = ntohs(CONST_VESION);
}

cmsg::~cmsg(void)
{
}

cmsg*
cmsg::Instance()
{
    if (obj_msg == NULL) {
        obj_msg = new cmsg();
    }

    return obj_msg;
}

bool
cmsg::pro_register_new(int idroom, int appid, const char* auth)
{
    int auth_len = strlen(auth);
    bool bret = false;
    char *p = NULL;
    cJSON *chatjson = NULL;
    if (auth_len > MAX_SIZE_BUF - 20) {
        goto release_mem;
    }
    msg_head.usOp = MSG_AUTH_WITH_ROOM;
    chatjson = cJSON_CreateObject();
    cJSON_AddNumberToObject(chatjson, "appid", appid);
    cJSON_AddNumberToObject(chatjson, "roomid", idroom);
    cJSON_AddStringToObject(chatjson, "auth", auth);
    cJSON_AddStringToObject(chatjson, "cid", "2");
    p = cJSON_PrintUnformatted(chatjson);
    if (NULL == p) {
        goto release_mem;
    }

    set_content_size(strlen(p));
    strcpy(ps_data->data, p);
    bret = true;
release_mem:
    if (chatjson) {
        cJSON_Delete(chatjson);
        chatjson = NULL;
    }
    if (p) {
        free(p);
        p = NULL;
    }
    return bret;
}

bool
cmsg::sub_room(const char* data, int size)
{
    msg_head.usOp = MSG_AUTH_WITH_ROOM;
    memcpy(ps_data->data, data, size);
    set_content_size(size);
    return true;
}

bool
cmsg::pro_chat_data(const char* data, int size)
{
    msg_head.usOp = MSG_PRIVATE_CHAT;
    memcpy(ps_data->data, data, size);
    set_content_size(size);
    return true;
}

bool
cmsg::pro_barrage(const char* strbarrage, int size)
{
    if (NULL == strbarrage) {
        return false;
    }

    msg_head.usOp = MSG_BARRAGE_PUBLIC;
    memcpy(ps_data->data, strbarrage, size);
    set_content_size(size);
    return true;
}

bool
cmsg::pro_heartbeat()
{
    msg_head.usOp = MSG_HEARTBEAT;
    set_content_size(0);

    return true;
}
bool
cmsg::init(const char* szbuf, int size)
{
    if (NULL == szbuf || size < 1 || size > MAX_SIZE_BUF) {
        return false;
    }

    memcpy(this->szbuf, szbuf, size);
    return true;
}

void
cmsg::process(CRoom* roomobj)
{
    switch (this->msg_head.usOp) {
    case MSG_AUTH_ROOM_ACK: {
        int room_id = ntohl(register_ret->room_id);
        if (room_id > 0) {
            roomobj->set_room_id(room_id);
            roomobj->set_status(true);
            CClientData::Instance()->enter_room_call(roomobj->get_roomid(), ENTER_OK);
            roomobj->start_keeplive();
        } else {
            roomobj->set_recon(false);
            roomobj->set_status(false);
            CClientData::Instance()->enter_room_call(roomobj->get_roomid(), ENTER_AUTH_ERROR);
            roomobj->destroy_room();
        }
    }
    break;
    case MSG_HEARTBEAT:
        //CClientData::Instance()->update_heartbeat_time();
        break;
    case MSG_BARRAGE_PUBLIC: {
        int size = this->get_content_size();
        ps_data->data[size] = '\0';
        CClientData::Instance()->rev_barrage(roomobj->get_roomid(), ps_data->data, size);
    }
    break;
    case MSG_PRIVATE_CHAT: {
        int size = this->get_content_size();
        ps_data->data[size] = '\0';
        CClientData::Instance()->rev_private_chat(roomobj->get_roomid(), ps_data->data);
    }
    break;
    default:
        break;
    }
}

