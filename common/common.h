/*
 * @file:        xxx.c
 * @author:      guangjieMVP
 * @version:     v1.00.00
 * @date:        2020-xx-xx
 * @github:      https://github.com/guangjieMVP
 * @brief: 
*/
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
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

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
    BROADCAST_CHAT_CMD     = 0x115200,
    PRIVATE_CHAT_CMD       = 0x110600,
    REGISTER_CMD           = 0x100502,
    LOGIN_CMD              = 0x100503,
    LOGOUT_CMD             = 0x100504,
    LIST_ALL_USER_CMD      = 0x120405,
    LIST_ONLINE_USER_CMD   = 0x120406,
};

enum CmdReply {
    REPLY_NONE                    = 0xA55A00,

    REPLY_REGSITER_SUCCESS         = 0xA55A01,
    REPLY_REGSITER_FAILD           = 0xA55A02,

    REPLY_LOGIN_SUCCESS            = 0xA55A03,
    REPLY_LOGIN_FAILD              = 0xA55A04,

    REPLY_LOGOUT_SUCCESS           = 0xA55A05,        //注销失败
    REPLY_LOGOUT_FAILD             = 0xA55A06,        //注销成功
 
    REPLY_PRIVATE_SUCCESS          = 0xA55A07,      //私聊成功
    REPLY_PRIVATE_FAILD            = 0xA55A08,
    
    REPLY_ALL_USER_SUCCESS         = 0xA55A09,
    REPLY_ALL_USER_FAILD           = 0xA55A0a,

    REPLY_ONLINE_USER_SUCCESS      = 0xA55A0b,
    REPLY_ONLINE_USER_FAILD        = 0xA55A0c,

    REPLY_BROADCAST_SUCCESS      = 0xA55A0D,
    REPLY_BROADCAST_FAILD        = 0xA55A0E,
};

/* C/S 通信协议 */
struct Protocol {
    int cmd;          //命令
    int cmd_reply;    //命令应答信息
    char name[32];   
    char msg[4096];
};

/* 命令处理 */
struct cmd {
    int cmd;
    void (*cmd_callback)(void);
};

void cmd_handle(struct cmd *cmds, uint32_t len, int cmd);

#endif /* _COMMON_H_  */

