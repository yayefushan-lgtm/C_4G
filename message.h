// message.h （ヘッダファイル）
#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdbool.h>

// 構造体の定義とtypedef

typedef struct
{
    char imsi[16];
    char auth_apn[32];
    char auth_key[16];
} SubscriberInfo;

typedef struct
{
    char ip[16];
    bool in_use;
} IP_UseState;

typedef struct
{
    int pgw_id;
    char apn[32];
    int pgw_teid;
    IP_UseState array[2];
} PGW;

typedef enum
{
    SESSION_INACTIVE,
    SESSION_ACTIVE
} SessionState;

typedef struct
{
    int sgw_id;
    int s1u_rx_teid;
    int pgw_side_teid;
    int bearer_id;
} SGW;

typedef struct
{
    int bearer_id;
    int qos;
    int sgw_teid;
    int pgw_teid;
} Bearer;

typedef struct
{
    char imsi[16];
    char apn[32];
    char ue_ip[16];
    Bearer bearer;
    SessionState state;
} Session;

#endif // _MESSAGE_H_
