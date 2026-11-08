#include <string.h>
#include <stdio.h>
#include "mmeAttach.h"
#include "message.h"

static ErrorCode hss_authenticate(const HSS *hss, const UE *ue);

int mme_attach(MME *mme, HSS *hss, UE *ue)
{
    if (strlen(ue->imsi) >= sizeof(mme->imsi)) {
        return -1;
    }
    strcpy(mme->imsi, ue->imsi);
    ue->state = ATTACHING;

    mme->auth_result = hss_authenticate(hss, ue);

    switch (mme->auth_result) 
    {
        case SUCCESS:
            break;
        case IMSI_MISMATCH:
            printf("IMSIが一致しません\n");
            break;
        case APN_DENIED:
            printf("APNが許可されていません\n");
            break;
        default:
            printf("不明なエラーです\n");
            break;
    }
    if (mme->auth_result != SUCCESS)
    {
        ue->state = DETACHED;
        mme->selected_sgw_id = 0;
        return mme->auth_result;
    }

    mme->selected_sgw_id = 1;
    return mme->auth_result;
}

static ErrorCode hss_authenticate(const HSS *hss, const UE *ue)
{

    for (size_t i = 0; i < sizeof(hss->subscribers) / sizeof(hss->subscribers[0]); i++)
    {
        if (strcmp(hss->subscribers[i].imsi, ue->imsi) == 0)
        {
            if (strcmp(hss->subscribers[i].auth_apn, ue->apn) == 0)
            {
                return SUCCESS;
            }
            return APN_DENIED;
        }

    }
    return IMSI_MISMATCH;
}
