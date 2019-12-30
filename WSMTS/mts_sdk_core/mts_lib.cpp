#include "stdafx.h"
#include "mts_lib.h"
#include "thread.h"
#include "define.h"
#include "event_mgr.h"
#include "clientdata.h"
#include "event2/event.h"
#include "cjson.h"
#include "common.h"
#include "room.h"
#include <pthread.h>

bool debug_on = false;

bool
init(const char* gdm_server, int port, int appid)
{
    if (is_string_empty(gdm_server)) {
        return false;
    }

    //已经初始化过了
    base_event_t *evbase = CClientData::Instance()->get_base_event();
    if (evbase->evbase != NULL) {
        return false;
    }

    if (!init_event()) {
        finish();
        return false;
    }

    //创建线程
    if (!create_thread()) {
        finish();
        return false;
    }

    CClientData::Instance()->set_base_info(gdm_server, port, appid);
    return true;
}


void
set_callback(Con_Ballback con_cb,
             Discon_Callback discon_cb,
             Enter_Room_Callback enter_room_cb,
             Barrage_Callback barrage_cb,
             Chat_Callback chat_cb)
{
    CClientData::Instance()->set_cb(con_cb, discon_cb, enter_room_cb, barrage_cb, chat_cb);
}


bool
enter_room(int room_id, const char* authority)
{
    if (room_id == 0 || room_id == -1) {
        return false;
    }

    if (is_string_empty(authority)) {
        CClientData::Instance()->setauth("");
    } else {
        CClientData::Instance()->setauth(authority);
    }

    base_event_t *evbase = CClientData::Instance()->get_base_event();
    int ret = event_base_once(evbase->evbase, -1, EV_TIMEOUT, enter_room_cb, (void *)room_id, NULL);
    return (ret == 0);
}

bool
send_barrage(int room_id, const char* szbuf, int size)
{
    if (size < 1 || NULL == szbuf) {
        return false;
    }

    struct evbuffer *buf;
    buf = create_evbuffer(room_id, (const char*)szbuf, size);
    base_event_t *evbase = CClientData::Instance()->get_base_event();
    int ret = event_base_once(evbase->evbase, -1, EV_TIMEOUT, send_barrage_cb, buf, NULL);
    if (ret != 0) {
        evbuffer_free(buf);
    }
    return (ret == 0);
}

bool
send_private_chat(int room_id, const char *send_cid, const char *recv_cid, const char* szbuf, int size)
{
    if (size < 1 || NULL == szbuf) {
        return false;
    }

    int ret = 1;
    cJSON *chatjson = NULL;
    char *content = (char *)malloc(size + 1);
    memcpy(content, szbuf, size);
    content[size] = '\0';

    chatjson = cJSON_CreateObject();
    cJSON_AddStringToObject(chatjson, "recv_cid", recv_cid);
    cJSON_AddStringToObject(chatjson, "send_cid", send_cid);
    cJSON_AddStringToObject(chatjson, "content", content);
    char *p = cJSON_PrintUnformatted(chatjson);
    if (NULL == p) {
        goto release_mem;
    }

    struct evbuffer *buf;
    base_event_t *evbase;
    buf = create_evbuffer(room_id, (const char*)p, strlen(p));
    evbase = CClientData::Instance()->get_base_event();
    ret = event_base_once(evbase->evbase, -1, EV_TIMEOUT, send_private_chat_cb, buf, NULL);
    if (ret != 0) {
        evbuffer_free(buf);
    }

release_mem:
    if (chatjson) {
        cJSON_Delete(chatjson);
    }
    if (p) {
        free(p);
    }
    free(content);
    return (ret == 0);
}

bool
leave_room(int room_id)
{
    base_event_t *evbase = CClientData::Instance()->get_base_event();
    int ret = event_base_once(evbase->evbase, -1, EV_TIMEOUT, leave_room_cb, (void *)room_id, NULL);
    return (ret == 0);
}

void
finish()
{
    base_event_t *evbase = CClientData::Instance()->get_base_event();
    if (evbase->evbase) {
        event_base_loopexit(evbase->evbase, NULL);
    }

    pthread_t pid = CClientData::Instance()->getThreadPid();
    if ((unsigned long)pid > 0) {
        pthread_join(pid, NULL);
    }

    if (evbase->evbase) {
        event_base_free(evbase->evbase);
        evbase->evbase = NULL;
    }

    CClientData::Instance()->setThreadPid(0);
}

void
set_debug_on_off(bool value)
{
    debug_on = value;
}

const char *
get_mts_sdk_version()
{
    return SDK_VERSION;
}

