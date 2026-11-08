#ifndef _MMEATTACH_H_
#define _MMEATTACH_H_

#include "message.h"

typedef enum
{
    DETACHED,
    ATTACHING,
    ATTACHED
} UeState;

typedef struct
{
    char imsi[16];
    int auth_result;
    int selected_sgw_id;
} MME;

typedef struct
{
    SubscriberInfo subscribers[3];
} HSS;

typedef struct
{
    char imsi[16];
    char apn[32];
    UeState state;
    char ip[16];
    int bearer_id;
} UE;

typedef enum
{
    SUCCESS,
    IMSI_MISMATCH,
    APN_DENIED
}ErrorCode;

int mme_attach(MME *mme, HSS *hss, UE *ue);

#endif // _MMEATTACH_H_
