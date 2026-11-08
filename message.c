#include <string.h>
#include <stdio.h>
#include "message.h"
#include "mmeAttach.h"


int pgw_allocate_ip(PGW *pgw, UE *ue)
{

    if (strcmp(ue->apn, pgw->apn) != 0) {
        return -1;    
    }

    for (size_t i = 0; i < sizeof(pgw->array)/sizeof(pgw->array[0]); i++)
    {
        if (pgw->array[i].in_use == false)
        {
            //コピー先の容量を確認してからコピーを始める（ue->ip）
            if (sizeof(ue->ip) < strlen(pgw->array[i].ip) + 1) 
            {
                return -1;
            }
            strcpy(ue->ip, pgw->array[i].ip);
            pgw->array[i].in_use = true;
            return 0;
        }
    }

    return -1;
}

Bearer create_bearer(SGW *sgw, PGW *pgw)
{
    Bearer bearer;

    bearer.bearer_id = sgw->bearer_id;
    bearer.qos = 9;
    bearer.sgw_teid = sgw->s1u_rx_teid;
    bearer.pgw_teid = pgw->pgw_teid;

    return bearer;
}

Session create_session(UE *ue, Bearer bearer)
{
    Session session;

    strcpy(session.imsi, ue->imsi);
    strcpy(session.apn, ue->apn);
    strcpy(session.ue_ip, ue->ip);
    session.bearer = bearer;
    session.state = SESSION_ACTIVE;

    ue->state = ATTACHED;
    ue->bearer_id = bearer.bearer_id;

    return session;
}

void print_session(const Session *session)
{
    printf("=== LTE Attach Result ===\n");
    printf("IMSI: %s\n", session->imsi);
    printf("APN: %s\n", session->apn);
    printf("UE IP: %s\n", session->ue_ip);
    printf("Bearer ID: %d\n", session->bearer.bearer_id);
    printf("QoS: %d\n", session->bearer.qos);
    printf("SGW TEID: %d\n", session->bearer.sgw_teid);
    printf("PGW TEID: %d\n", session->bearer.pgw_teid);
    printf("Session State: %d\n", session->state);
}

void cleanup_failed_attach(UE *ue, PGW *pgw) {
    for (size_t i = 0; i < sizeof(pgw->array) / sizeof(pgw->array[0]); i++) {
        if (pgw->array[i].in_use && strcmp(pgw->array[i].ip, ue->ip) == 0) {
            pgw->array[i].in_use = false;
            ue->ip[0] = '\0';
            break;
        } 
    }
    ue->bearer_id = 0;
    ue->state = DETACHED;
    return;
}

int main(void)
{
    // 構造体の実体の生成と初期化(代入)
    UE ue = {
        "440100000000001",
        "internet",
        DETACHED,
        "",
        0};

    UE ue2 = {
        "440100000000002",
        "internet",
        DETACHED,
        "",
        0};

    UE pool[2] = {ue, ue2};

    HSS hss = {
        .subscribers = 
        { 
            {.imsi = "440100000000001", .auth_apn = "internet", .auth_key = "secret"},
            {.imsi = "440100000000002", .auth_apn = "internet", .auth_key = "secret"}
        }
    };

    MME mme = {
        "",
        0,
        0
    };

    SGW sgw = {
        1,
        1001,
        2001,
        5};

    PGW pgw = {
        1,
        "internet",
        2001,
        {
            {"10.0.0.2", false},
            {"10.0.0.3", false}
        }
        };
    
    for (size_t i = 0; i < sizeof(pool) / sizeof(pool[0]); i++)
    {
        UE *current_ue = &pool[i];
        if (mme_attach(&mme, &hss, current_ue) != 0) {
            return 1;
        }


        if (current_ue->state != ATTACHING) {
            return 1;
        }
        
        if (pgw_allocate_ip(&pgw, current_ue) != 0) {
            goto fail;
        }
                
        if (sgw.bearer_id <= 0) {
            goto fail;
        }
        
        if (sgw.pgw_side_teid != pgw.pgw_teid) {
            goto fail;
        }
        
        if (sgw.s1u_rx_teid == 0 || pgw.pgw_teid == 0) {
            goto fail;
        }
        
        Bearer bearer = create_bearer(&sgw, &pgw);

        Session session = create_session(current_ue, bearer);

        print_session(&session);

        continue;
        
        fail:
            cleanup_failed_attach(current_ue, &pgw);
            return 1;
    }
    
    return 0;
}
