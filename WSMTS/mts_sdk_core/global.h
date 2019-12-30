#ifndef GLOBAL_H_
#define GLOBAL_H_

typedef enum {
    PRI_QUICK = 1,
    PRI_FAST = 2,
    PRI_COMMON = 3,
    PRI_SLOW = 4,
} BARRAGE_PRIORITY;

extern bool debug_on;

#define debug(fmt, args...) \
    if (debug_on) printf(fmt, ##args)


#endif
