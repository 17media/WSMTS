#include "stdafx.h"
#include "ipcontainer.h"
#include "define.h"

void
container_init(ipcontainer_t* pContain)
{
    if (NULL == pContain) {
        return;
    }
    memset(pContain->ips, 0, sizeof(pContain->ips));
    pContain->size = 0;
}
bool
container_add_ip(ipcontainer_t* pContainer, const char* ip)
{
    if (NULL == pContainer || pContainer->size < 0 || pContainer->size >= MAXIP_NUM) {
        return false;
    }

    UINT nIP = inet_addr(ip);
    if (nIP < 1) {
        return false;
    }
    pContainer->ips[pContainer->size] = nIP;
    pContainer->size += 1;
    return true;
}

const char*
container_get_ip(ipcontainer_t* pContainer, int index)
{
    if (index < 0) {
        index = 0;
    }
    static char strIP[IP_LEN];
    memset(strIP, 0, IP_LEN);
    if (NULL == pContainer) {
        return NULL;
    }
    if (pContainer->size < 1) {
        return NULL;
    }

    size_t i = index % pContainer->size;
    UINT	nIP = pContainer->ips[i];
    struct in_addr in;
    memcpy(&in, &nIP, 4);
    char* a1 = inet_ntoa(in);
    snprintf(strIP, IP_LEN, "%s", a1);
    return strIP;
}

void
container_clear(ipcontainer_t* pContainer)
{
    container_init(pContainer);
}

