/*
 * @file:        xxx.c
 * @author:      guangjieMVP
 * @version:     v1.00.00
 * @date:        2020-xx-xx
 * @github:      https://github.com/guangjieMVP
 * @brief: 
*/

#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/types.h>          
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h> 
#include <signal.h>

#define SERVER_PORT      8888                        //端口号
#define ARR_SIZE(a)      (sizeof(a)/sizeof(a[0]))

/* user infomation */
struct userinformation {
    int fd;                  //用户客户端的描述符
    char name[32];           //用户名
    char password[32];       //用户密码
};
typedef struct userinformation userinfo_t;

/* cmds */
enum CmdCode {
    BROADCAST_CHAT_CMD = 0x115200,
    PRIVATE_CHAT_CMD   = 0x110600,
    REGISTER_CMD       = 0x100502,
    LOGIN_CMD          = 0x100503,
    LOGOUT_CMD         = 0x100504,
    LIST_ONLINE_USER   = 0x120406,
};

enum CmdReply {
    REPLY_NONE             = 0x00,
    REPLY_REGSITER_SUCCESS = 0x01,
    REPLY_REGSITER_FAILD   = 0x02,
    REPLY_LOGIN_SUCCESS    = 0x04,
    REPLY_LOGIN_FAILD      = 0x05,
};

/* C/S 通信协议 */
struct Protocol {
    int cmd;          //命令
    int cmd_reply;    //命令应答信息
    char name[32];   
    char msg[1024];
};

/* 命令处理 */
struct cmd {
    int cmd;
    void (*cmd_callback)(void);
};

void cmd_handle(struct cmd *cmds, uint32_t len, int cmd);

#endif /* _COMMON_H_  */

