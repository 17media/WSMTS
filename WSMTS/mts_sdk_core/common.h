#ifndef __COMMON_H__
#define __COMMON_H__
#include "define.h"
#include "ipcontainer.h"
#include "event2/buffer.h"


//获取系统号
extern const char*	get_sysname();
//计算urlencode
extern int			URLEncode(const char* str, const int strSize, char* result, const int resultSize);
//out_ip 长度不小于 IP_LEN 字节
extern bool			get_ip_by_dns(const char* dns, ipcontainer_t* pContainer);
//获取全局唯一id，非线程安全，从1自增
extern int			product_unique_id();

inline bool
is_string_empty(const char *send_cid)
{
    return (send_cid == NULL || strlen(send_cid) < 1);
}
extern int calc_delay(int times);
extern bool strcopy(char* tg, const char* sr, size_t max_size);

//////////////////////////////////////////////////////////////////////////
//自动释放对象
class CAutoFree
{
public:
    CAutoFree(struct evbuffer *pBuf)
    {
        if (pBuf == NULL) {
            return;
        }

        this->m_pBuf = pBuf;
    }
    virtual
    ~CAutoFree()
    {
        if (m_pBuf == NULL) {
            return;
        }

        evbuffer_free(this->m_pBuf);
    }
private:
    struct evbuffer* m_pBuf;
};
#endif
