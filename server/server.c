#include "common.h"
#include "database.h"


#define BACKLOG      10


static  struct Protocol ptl = {0};
static  int    socket_fd;

static void _broadcast_chat_cmd_cb(void)
{
    printf("广播命令\n");
}

static void _private_chat_cmd_cb(void)
{
    printf("私聊命令\n");
}

static void _register_cmd_cb(void)
{
    int fd = -1;
    printf("注册命令\n");

    if (!check_is_username_exist(ptl.name))   //该用户已经存在，不能注册
    {
        int ret = insert_usertbl_all_field(fd, ptl.name, ptl.msg);
        // printf("ret = %d\n", ret);
        if (!ret)  
        {
            ptl.cmd_reply = REPLY_REGSITER_FAILD;
            printf("注册失败\n");
        }
        else
        {
            ptl.cmd_reply = REPLY_REGSITER_SUCCESS;
            printf("注册成功\n");
        }
    }
    else
    {
        ptl.cmd_reply = REPLY_REGSITER_FAILD;
        printf("注册失败\n");
    }

    send(socket_fd, &ptl, sizeof(struct Protocol), 0); 
}

static void _login_cmd_cb(void)
{
    printf("登录命令\n");
    if (check_username_and_password_exist(ptl.name, ptl.msg))   //数据库中存在该用户
    {                                           
                                                  //登录
        ptl.cmd_reply = REPLY_LOGIN_SUCCESS;                                         
    }
    else
    {
        ptl.cmd_reply = REPLY_LOGIN_FAILD;
    }
    send(socket_fd, &ptl, sizeof(struct Protocol), 0); 
}

static void _logout_cmd_cb(void)
{
    printf("注销命令\n");
}

static void _listonline_user_cmd_cb(void)
{
    printf("列出在线用户命令\n");
}

struct cmd cmdlist[] = {
    {.cmd = BROADCAST_CHAT_CMD, .cmd_callback = _broadcast_chat_cmd_cb},
    {.cmd = PRIVATE_CHAT_CMD,   .cmd_callback = _private_chat_cmd_cb},
    {.cmd = REGISTER_CMD,       .cmd_callback = _register_cmd_cb},
    {.cmd = LOGIN_CMD,          .cmd_callback = _login_cmd_cb},
    {.cmd = LOGOUT_CMD,         .cmd_callback = _logout_cmd_cb},
    {.cmd = LIST_ONLINE_USER,   .cmd_callback = _listonline_user_cmd_cb}, 
};

static int thread_exit_val; 

void *thread_recv(void *para)
{
    int recv_len;
    // uint8_t recv_buf[1000];
    socket_fd = *((int *)para);
    free(para);
    
    while (1) 
    {
        if ((recv_len = recv(socket_fd, &ptl, sizeof(struct Protocol), 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        if (recv_len == -1)   //客户端出错
        {
            close(socket_fd); 
            // exit(1);
            thread_exit_val = 1;
            pthread_exit(&thread_exit_val);
            // break; 
        } 
        if (recv_len == 0)    //客户端主动断开连接
        {
            close(socket_fd);
            thread_exit_val = 1;
            pthread_exit(&thread_exit_val);
            // break;
            // exit(1); 
        }

        // info = (userinfo_t *)*((**ptl.msg).data);
        printf("cmd = %x\n", ptl.cmd);
        printf("name : %s\n", ptl.name);
        printf("password : %s\n", ptl.msg); 

        cmd_handle(cmdlist, ARR_SIZE(cmdlist), ptl.cmd);
        
//        ptl.cmd_reply = REPLY_REGSITER_SUCCESS;
//        send(socket, &ptl, sizeof(struct protocol), 0);
        // recv_buf[recv_len] = '\0';
        // printf("Get Message From Client : %s\n", recv_buf);
        // if (send(socket, recv_buf, strlen(recv_buf), 0) == -1) 
        // {
        //     printf("Send Error\n");
        //     exit(1);
        // }
       
    }
}


int main(int argc, char **argv)
{
    int ret;
	int addr_len; 

	int sock_server;
	int sock_client;
	struct sockaddr_in sock_server_addr;
	struct sockaddr_in sock_client_addr;
	
    pthread_t thread;
    int *soket_id;

	int client_num = -1;

    signal(SIGPIPE, SIG_IGN); 

	sock_server = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sock_server)
	{
		perror("socket error!\n");
		return -1;
	}
    
	sock_server_addr.sin_family      = AF_INET;
 	sock_server_addr.sin_addr.s_addr = INADDR_ANY;  //任意地址都可以链接
    sock_server_addr.sin_port        = htons(SERVER_PORT);  /* host to net, short */
	memset(sock_server_addr.sin_zero, 0, 8);

	ret = bind(sock_server, (const struct sockaddr *)&sock_server_addr, sizeof(struct sockaddr));
    if (-1 == ret)
    {
		perror("bind error!\n");
		return -1;
	}

    ret = listen(sock_server, BACKLOG);
    if (ret == -1)
	{
		perror("listen error!\n");
		return -1;
	}

    ret = database_init();
    if (ret == -1)
    {
        printf("database init faild!\n");
        return -1;
    }
        
	while (1)
	{
		addr_len = sizeof(struct sockaddr_in);
		if ((sock_client = accept(sock_server, (struct sockaddr *)&sock_client_addr, &addr_len)) == -1)
        {
            printf("Accept error\n");
            exit(1);            
        }

        soket_id = (int *)malloc(sizeof(int));
        *soket_id = sock_client;

        ret = pthread_create(&thread, NULL, thread_recv, (void *)soket_id);  //创建线程
        if (ret < 0)
        {
            perror("pthread_create");
            return -1;
        }
	}
//	close(sock_server);
	return 0;
}