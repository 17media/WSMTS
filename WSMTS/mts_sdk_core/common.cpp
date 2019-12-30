#include "stdafx.h"
#include "common.h"

int gUninID = 1;
bool
get_ip_by_dns(const char* dns, ipcontainer_t* pContainer)
{
    if (dns == NULL || NULL == pContainer) {
        return false;
    }
    char out_ip[IP_LEN] = {0};
    char **pptr;
    struct hostent* hptr = gethostbyname(dns);
    if (hptr == NULL || (pptr = hptr->h_addr_list) == NULL) {
        return false;
    }

    switch (hptr->h_addrtype) { //根据地址类型，将地址打印出来
    case AF_INET:
    case AF_INET6:
        for (; *pptr != NULL; pptr++) {
            inet_ntop(hptr->h_addrtype, *pptr, out_ip, IP_LEN);   //inet_ntop: 将网络字节序的二进制转换为文本字符串的格式
            container_add_ip(pContainer, out_ip);
        }
        break;
    default:
        return false;
    }
    return true;
}

const char*
get_sysname()
{
    static char* strRet = new char[128];
    memset(strRet, 0, 128);
#ifdef WIN32
    snprintf(strRet, 128, "%s", "windows");
#else
    struct utsname u;
    if (uname(&u) != -1) {
        snprintf(strRet, 128, "%s", u.sysname);
    }
#endif

    for (int i = 0; i < 128; i++) {
        if (0 == strRet[i]) {
            break;
        } else if (strRet[i] == ' ') {
            strRet[i] = '_';
        }
    }

    return strRet;
}

extern int
URLEncode(const char* str, const int strSize, char* result, const int resultSize)
{
    int i;
    int j = 0;//for result index
    char ch;

    if ((str == NULL) || (result == NULL) || (strSize <= 0) || (resultSize <= 0)) {
        return 0;
    }

    for ( i = 0; (i < strSize) && (j < resultSize); ++i) {
        ch = str[i];
        if (((ch >= 'A') && (ch < 'Z')) ||
            ((ch >= 'a') && (ch < 'z')) ||
            ((ch >= '0') && (ch < '9'))) {
            result[j++] = ch;
        } else if (ch == ' ') {
            result[j++] = '+';
        } else if (ch == '.' || ch == '-' || ch == '_' || ch == '*') {
            result[j++] = ch;
        } else {
            if (j + 3 < resultSize) {
                sprintf(result + j, "%%%02X", (unsigned char)ch);
                j += 3;
            } else {
                return 0;
            }
        }
    }

    result[j] = '\0';
    return j;
}

int
product_unique_id()
{
    return gUninID++;
}

int
calc_delay(int times)
{
    switch (times) {
    case 0:
    case 1:
        return 500;
    case 2:
        return 1000;
    case 3:
        return 2000;
    case 4:
        return 5000;
    default:
        return 7000;
    }
}

bool
strcopy(char* tg, const char* sr, size_t max_size)
{
    if (NULL == tg || NULL == sr || max_size < 1) {
        return false;
    }

    int cpylen = strlen(sr) >= max_size ? max_size - 1 : strlen(sr);
    memcpy(tg, sr, cpylen);
    tg[cpylen] = 0;
    return true;
}
