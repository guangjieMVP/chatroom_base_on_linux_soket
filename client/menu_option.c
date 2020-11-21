#include "menu_option.h"
#include "client.h"


enum {
    OP_EXIT = 0x00,
    OP_REGISTER = 0x01,
    OP_LOGIN = 0x02,
    OP_LOGOUT = 0x03,
    OP_BROADCAST = 0x04,
    OP_PRIVATE = 0x05,
    OP_ALLUSER = 0x06,
};


void menu_init()
{
    printf("\n*************1、注册*******************");
    printf("\n*************2、登录*******************");
    printf("\n*************6、显示所有用户************");
    printf("\n*************0、退出*******************");
    printf("\n");
    printf("请输入你的选择：\n");
}

/* 登录后的界面 */
void login_menu(void)
{
    printf("\n*************3、注销*******************");
    printf("\n*************4、公聊*******************");
    printf("\n*************5、私聊*******************");
    printf("\n*************7、显示所有在线用户********");
    printf("\n");
    printf("请输入你的选择：\n");
}


int menu_option_handle(struct option *ops, uint32_t len, int op)
{
    int index;
    int ret = 0;

    if (!ops) 
        return -1;
    
    for (index = 0; index < len; index++)
    {
        if (ops[index].op == op)
        {
            if (ops[index].op_callback)
                ret = ops[index].op_callback();
            break;
        }
    }

    return ret;
}

int input_menu_option(void)
{
    int op;
    char num[20];

    scanf("%s", num);
    op = atoi(num);
    return op;
}

static void regsiter_success_cb(void)
{
    printf("恭喜您!!!注册成功\n");
}

static void regsiter_failed_cb(void)
{
    printf("该用户已存在!!!注册失败\n");
}

static void login_success_cb(void)
{
    client_sem_post(&g_client_sem.login_sem);   //登录成功，发送登录成功信号量
    printf("登录成功\n");
}

static void login_failed_cb(void)
{
    printf("登录失败, 用户名或密码错误\n");
}

static void logout_success_cb(void)
{
    printf("注销成功\n");
}

static void logout_failed_cb(void)
{
    printf("注销失败\n");
}

static void list_all_user_success_cb(void)
{
    printf("用户名：");
}

struct cmd cmd_reply_list[] = {
    {.cmd = REPLY_REGSITER_SUCCESS, .cmd_callback = regsiter_success_cb},
    {.cmd = REPLY_REGSITER_FAILD,   .cmd_callback = regsiter_failed_cb},
    {.cmd = REPLY_LOGIN_SUCCESS,    .cmd_callback = login_success_cb},
    {.cmd = REPLY_LOGIN_FAILD,      .cmd_callback = login_failed_cb},
    {.cmd = REPLY_LOGOUT_SUCCESS,   .cmd_callback = logout_success_cb},
    {.cmd = REPLY_LOGOUT_FAILD,     .cmd_callback = logout_failed_cb},
    {.cmd = REPLY_ALL_USER_SUCCESS,  .cmd_callback = list_all_user_success_cb},
};


static int op_logout(void)
{
    struct Protocol ptl;
    int len;

    ptl.cmd = LOGOUT_CMD;             
    ptl.cmd_reply = REPLY_NONE;

    extern int sock_client; 
    len = send(sock_client, &ptl, sizeof(struct Protocol), 0);    //给服务端发送注销登录信息
    printf("注销\n");

    return OP_LOGOUT;
}

static int op_broadcast(void)
{   
    printf("公聊\n");

    return OP_BROADCAST;
}

static int op_private(void)
{   
    printf("私聊\n");

    return OP_PRIVATE;
}



struct option login_ops[] = {
    {.op = OP_LOGOUT, op_logout},
    {.op = OP_BROADCAST, op_broadcast},
    {.op = OP_PRIVATE, op_private},
    
};


static int op_exit(void)
{
    extern int sock_client;
    close(sock_client);
	exit(0);
    // return 0;
}

static int op_register(void)
{
    struct Protocol ptl;
    struct Protocol reply_ptl;

    printf("请输入用户名：\n");
    scanf("%s", ptl.name);
    printf("请输入用户密码：\n");
    scanf("%s", ptl.msg);

    ptl.cmd = REGISTER_CMD;
    ptl.cmd_reply = REPLY_NONE;

    extern int sock_client; 
    int len = send(sock_client, &ptl, sizeof(struct Protocol), 0);    //给服务端发送注册信息
    printf("注册\n");

    // recv(sock_client, &reply_ptl, sizeof(struct protocol), 0);
    // if (reply_ptl.cmd_reply != REPLY_REGSITER_SUCCESS)
    // {
    //     printf("注册失败\n");
    // }
    // else
    // {
    //     printf("注册成功\n");
    // }
    // getchar();
    // getchar();
}

static int op_login(void)
{
    int ret;
    struct Protocol ptl;

    printf("请输入用户名：\n");
    scanf("%s", ptl.name);
    printf("请输入用户密码：\n");
    scanf("%s", ptl.msg);

    ptl.cmd = LOGIN_CMD; 
    ptl.cmd_reply = REPLY_NONE;

    extern int sock_client; 
    int len = send(sock_client, &ptl, sizeof(struct Protocol), 0);    //向服务端请求登录
    printf("登录\n");

    client_sem_wait(&g_client_sem.login_sem);                         //等待成功登录的信号量 才能进入登录后的菜单
    while(1)
    {
        login_menu();
        int op = input_menu_option();
        ret = menu_option_handle(login_ops, ARR_SIZE(login_ops), op);
        if (ret == OP_LOGOUT)     //注销登录退出当前界面
        {
            break;
        }
    }

    return OP_LOGIN;
}

static int op_all_users(void)
{   
    printf("显示所有用户\n");

    struct Protocol ptl;
    int len;

    ptl.cmd = LIST_ALL_USER_CMD;             
    ptl.cmd_reply = REPLY_NONE;

    extern int sock_client; 
    len = send(sock_client, &ptl, sizeof(struct Protocol), 0);    //给服务端发送注销登录信息

    return OP_ALLUSER;
}

/* 菜单选项 */
struct option main_menu_ops[] = {
    {.op = OP_EXIT,     op_exit},
    {.op = OP_REGISTER, op_register},
    {.op = OP_LOGIN,    op_login},
    {.op = OP_ALLUSER,  op_all_users},
};


int cmd_reply_list_size = ARR_SIZE(cmd_reply_list);
int main_menu_ops_size  = ARR_SIZE(main_menu_ops);




