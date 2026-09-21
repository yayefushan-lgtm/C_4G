#include <string.h>
#include <stdio.h>

typedef enum
{
    DETACHED,
    ATTACHING,
    ATTACHED
} UeState;

typedef enum
{
    SESSION_INACTIVE,
    SESSION_ACTIVE
} SessionState;

typedef struct
{
    char imsi[16];
    char apn[32];
    UeState state;
    char ip[16];
    int bearer_id;
} UE;

typedef struct
{
    char registered_imsi[16];
    char allowed_apn[32];
    char auth_key[16];
} HSS;

typedef struct
{
    char imsi[16];
    int auth_result;
    int selected_sgw_id;
} MME;

typedef struct
{
    int sgw_id;
    int s1u_rx_teid;
    int pgw_side_teid;
    int bearer_id;
} SGW;

typedef struct
{
    int pgw_id;
    char apn[32];
    char allocated_ip[16];
    int pgw_teid;
} PGW;

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

int hss_authenticate(HSS *hss, UE *ue)
{
    if (strcmp(hss->registered_imsi, ue->imsi) == 0)
        if (strcmp(ue->apn, hss->allowed_apn) == 0)
        {
            return 0;
        }

    return -1;
}

int mme_attach(MME *mme, HSS *hss, UE *ue)
{
    if (strlen(ue->imsi) >= sizeof(mme->imsi)) {
        return -1;
    }
    strcpy(mme->imsi, ue->imsi);
    ue->state = ATTACHING;

    mme->auth_result = hss_authenticate(hss, ue);

    if (mme->auth_result != 0)
    {
        ue->state = DETACHED;
        mme->selected_sgw_id = 0;
        return -1;
    }

    mme->selected_sgw_id = 1;
    return 0;
}

int pgw_allocate_ip(PGW *pgw, UE *ue)
{
    if (strcmp(ue->apn, pgw->apn) != 0) {
        return -1;    
    }
    if (sizeof(ue->ip) < sizeof("10.0.0.2")) {
        return -1;
    }
    strcpy(ue->ip, "10.0.0.2");

    if (sizeof(pgw->allocated_ip) < strlen(ue->ip) + 1) {
        return -1;
    }

    strcpy(pgw->allocated_ip, ue->ip);

    return 0;
};

Bearer create_bearer(SGW *sgw, PGW *pgw)
{
    Bearer bearer;

    bearer.bearer_id = 5;
    bearer.qos = 9;
    bearer.sgw_teid = sgw->s1u_rx_teid;
    bearer.pgw_teid = pgw->pgw_teid;

    return bearer;
}

Session create_session(UE *ue, PGW *pgw, Bearer bearer)
{
    Session session;

    strcpy(session.imsi, ue->imsi);
    strcpy(session.apn, ue->apn);
    strcpy(session.ue_ip, pgw->allocated_ip);
    session.bearer = bearer;
    session.state = SESSION_ACTIVE;

    ue->state = ATTACHED;
    ue->bearer_id = bearer.bearer_id;

    return session;
}

void print_session(Session *session)
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

int main(void)
{
    UE ue = {
        "440100000000001",
        "internet",
        DETACHED,
        "",
        0};

    HSS hss = {
        "440100000000001",
        "internet",
        "secret"};

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
        "",
        2001};

    // Bearer bearer = {
        // 0,
        // 0,
        // 0,
        // 0};
    printf("%d", 1);
    if (mme_attach(&mme, &hss, &ue) != 0) {
        return 1;
    }
    
    if (pgw_allocate_ip(&pgw, &ue) != 0) {
        return 1;
    }
    
    if (sgw.bearer_id <= 0) {
        return 1;
    }
    if (sgw.pgw_side_teid != pgw.pgw_teid) {
        return 1;
    }

    if (sgw.bearer_id <= 0) {  
        return 1;
    }
    
    if (sgw.s1u_rx_teid == 0 || pgw.pgw_teid == 0) {
        return 1;
    }
    Bearer bearer = create_bearer(&sgw, &pgw);
    
    if (ue.state != ATTACHING) {
        return 1;
    }
    printf("%d", 1);
    Session session = create_session(&ue, &pgw, bearer);
    printf("%d", 2);
    print_session(&session);

    return 0;
}