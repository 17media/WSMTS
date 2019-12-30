#ifndef __IPCONTAINER_H__
#define __IPCONTAINER_H__
#include"stdafx.h"
/*
*本文件中的所有函数都是非线程安全，使用的时候请注意
*/
const int MAXIP_NUM = 50;
typedef struct ipcontainer {
    ULONG	ips[MAXIP_NUM];
    int		size;
} ipcontainer_t;

extern void			container_init(ipcontainer_t* pContain);
extern bool			container_add_ip(ipcontainer_t* pContainer, const char* ip);
extern const char*	container_get_ip(ipcontainer_t* pContainer, int index);
extern void			container_clear(ipcontainer_t* pContainer);
#endif

