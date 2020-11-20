#include "common.h"

struct option {
    int op;
    int (*op_callback)(void);
};

enum {
    OP_EXIT = 0,
    OP_REGISTER = 1,
    OP_LOGIN = 2,
    OP_LOGOUT = 3,
    OP_BROADCAST = 4,
    OP_PRIVATE = 5,
};

void regsiter_success_cb(void)
{
    printf("注册成功\n");
}

void regsiter_failed_cb(void)
{
    printf("注册失败\n");
}

void login_success_cb(void)
{
    printf("登录成功\n");
}

void login_failed_cb(void)
{
    printf("登录失败\n");
}


static struct cmd cmd_reply_list[] = {
    {.cmd = REPLY_REGSITER_SUCCESS, .cmd_callback = regsiter_success_cb},
    {.cmd = REPLY_REGSITER_FAILD,   .cmd_callback = regsiter_failed_cb},
    {.cmd = REPLY_LOGIN_SUCCESS,    .cmd_callback = login_success_cb},
    {.cmd = REPLY_LOGIN_FAILD,      .cmd_callback = login_failed_cb},
};

int op_logout(void)
{
    printf("注销\n");
}

int op_broadcast(void)
{   
    printf("公聊\n");
}


int op_private(void)
{   
    printf("私聊\n");
}

struct option login_option_list[] = {
    {.op = OP_LOGOUT, op_logout},
    {.op = OP_BROADCAST, op_broadcast},
    {.op = OP_PRIVATE, op_private},
};


int op_exit(void)
{
    extern int sock_client;
    close(sock_client);
	exit(0);
    return 0;
}

int op_register(void)
{
    struct protocol ptl;
    struct protocol reply_ptl;

    printf("请输入用户名：\n");
    scanf("%s", ptl.name);
    printf("请输入用户密码：\n");
    scanf("%s", ptl.msg);

    ptl.cmd = REGISTER_CMD;
    ptl.cmd_reply = REPLY_NONE;

    extern int sock_client; 
    int len = send(sock_client, &ptl, sizeof(struct protocol), 0);    //给服务端发送注册信息
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

void menu_option_handle(struct option *ops, uint32_t len, int op);
void login_menu(void);
int input_menu_option(void);

int op_login(void)
{
    struct protocol ptl;
    struct protocol reply_ptl;

    printf("请输入用户名：\n");
    scanf("%s", ptl.name);
    printf("请输入用户密码：\n");
    scanf("%s", ptl.msg);

    ptl.cmd = LOGIN_CMD;
    ptl.cmd_reply = REPLY_NONE;

    extern int sock_client; 
    int len = send(sock_client, &ptl, sizeof(struct protocol), 0);    //给服务端发送注册信息
    printf("登录\n");

                               //等待成功登录的信号量
    while(1)
    {
        login_menu();
        int op = input_menu_option();
        menu_option_handle(login_option_list, ARR_SIZE(login_option_list), op);
    }
    // recv(sock_client, &reply_ptl, sizeof(struct protocol), 0);
    // if (reply_ptl.cmd_reply != REPLY_REGSITER_SUCCESS)
    // {
    //     printf("注册失败\n");
    // }
    // else
    // {
    //     printf("注册成功\n");
    // }
}



/* 菜单选项 */
struct option option_list[] = {
    {.op = OP_EXIT, op_exit},
    {.op = OP_REGISTER, op_register},
    {.op = OP_LOGIN, op_login},
};


void menu_option_handle(struct option *ops, uint32_t len, int op)
{
    int index;

    if (!ops) return;
    
    for (index = 0; index < len; index++)
    {
        if (ops[index].op == op)
        {
            if (ops[index].op_callback)
                ops[index].op_callback();
            break;
        }
    }
}

void menu_init()
{
    printf("\n*************1、注册*******************");
    printf("\n*************2、登录*******************");
    printf("\n*************0、退出*******************");
    printf("\n");
    printf("请输入你的选择：\n");
}

void login_menu(void)
{
    printf("\n*************3、注销*******************");
    printf("\n*************4、公聊*******************");
    printf("\n*************5、私聊*******************");
    printf("\n");
    printf("请输入你的选择：\n");
}

int input_menu_option(void)
{
    int op;
    char num[20];

    scanf("%s", num);
    op = atoi(num);
    return op;
}

int global_socket;

void *thread_recv(void *para)
{
    struct protocol reply_ptl;
    int recv_len;
    int socket = *(int *)para;
    uint8_t recv_buf[1000];
    global_socket = socket;
    free(para);     //释放内存

    while (1) 
    {
        recv_len = recv(socket, &reply_ptl, sizeof(struct protocol), 0);
        if (recv_len <= 0)
        {
            close(socket);
            exit(1); 
        }
        
        cmd_handle(cmd_reply_list, ARR_SIZE(cmd_reply_list), reply_ptl.cmd_reply);  //处理应答的消息
    }
}

int sock_client;
struct sockaddr_in sock_server_addr;

int main(int argc, char **argv)
{
    int *pclient = NULL;
    pthread_t thread;

	int ret;

	unsigned char send_buf[1024];
	int send_len;

	if (argc != 2)
	{
		printf("Usage:\n");
		printf("%s <server_ip>\n", argv[0]);
		return -1;
	}

    signal(SIGPIPE, SIG_IGN);  
    signal(SIGINT, SIG_IGN);        //忽略ctrl + c发送的信号

	sock_client = socket(AF_INET, SOCK_STREAM, 0);    //创建套接字
	sock_server_addr.sin_family      = AF_INET;
	sock_server_addr.sin_port        = htons(SERVER_PORT);
 	if (0 == inet_aton(argv[1], &sock_server_addr.sin_addr))
 	{
		perror("invalid server_ip\n");
		return -1;
	}
	memset(sock_server_addr.sin_zero, 0, 8);
    
	ret = connect(sock_client, (const struct sockaddr *)&sock_server_addr, sizeof(struct sockaddr));	

	if (-1 == ret)
	{
		perror("connect error!\n");
		return -1;
	}

    pclient = (int *)malloc(sizeof(int));
    *pclient = sock_client;
    ret = pthread_create(&thread, NULL, thread_recv, (void *)pclient);  //创建负责接收线程
    if (-1 == ret)
    {
        perror("pthread_create");
        return -1;
    }

    int op;

	while (1) 
	{
        menu_init();
        op = input_menu_option();
        menu_option_handle(option_list, ARR_SIZE(option_list), op);
//         send_len = send(sock_client, send_buf, strlen(send_buf), 0);
//         if (send_len <= 0)
//         {
// //				close(sock_client);
//             return -1;
//         }
	}


    printf("退出\n");
    close(sock_client);
	return 0;
}