#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "common.h"

struct client_sem {
    sem_t login_sem;    //登录成功信号量
};

extern struct client_sem  g_client_sem;

void client_sem_post(sem_t *s);
void client_sem_wait(sem_t *s);

#endif