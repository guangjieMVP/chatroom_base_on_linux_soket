#include "client.h"
#include "menu_option.h"

struct client_sem  g_client_sem;

int client_sem_init(void)
{
    int ret;
    ret = sem_init(&g_client_sem.login_sem, 0, 0);

    return ret;
}

void client_sem_post(sem_t *s)
{
    sem_post(s);
}

void client_sem_wait(sem_t *s)
{
    sem_wait(s);
}

int global_socket;

void *thread_recv(void *para)
{
    struct Protocol reply_ptl;
    int recv_len;
    int socket = *(int *)para;
    uint8_t recv_buf[1000];
    global_socket = socket;
    free(para);     //释放内存

    while (1) 
    {
        recv_len = recv(socket, &reply_ptl, sizeof(struct Protocol), 0);
        if (recv_len <= 0)
        {
            close(socket);
            exit(1); 
        }
        
        cmd_handle(cmd_reply_list, cmd_reply_list_size, reply_ptl.cmd_reply);  //处理应答的消息
    }
}

int sock_client;
struct sockaddr_in sock_server_addr;

int main(int argc, char **argv)
{
    int *pclient = NULL;
    pthread_t thread;

	int ret;

	if (argc != 2)
	{
		printf("Usage:\n");
		printf("%s <server_ip>\n", argv[0]);
		return -1;
	}

    signal(SIGPIPE, SIG_IGN);  
    signal(SIGINT, SIG_IGN);            //忽略ctrl + c发送杀死终端的SIGINT信号  使用菜单提供的退出操作才能退出了客户端

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

    ret = client_sem_init();
    if (-1 == ret)
    {
        perror("client sem init failed\n");
        return -1;
    }

    int op;
	while (1) 
	{
        menu_init();
        op = input_menu_option();
        menu_option_handle(main_menu_ops, main_menu_ops_size, op);
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