/*
  * @file:        
  * @author:      guangjieMVP
  * @version:     v1.00.00
  * @date:        2020-xx-xx
  * @github:      https://github.com/guangjieMVP
  * @brief:       服务器模块开发
*/

#include <stdio.h>
#include "database.h"
#include "stdint.h"
#include "stdlib.h"

#define DATBASE_PATH    "chatroom.db"     //聊天室数据库
#define TABLE_NAME      "user"            //数据表名

static sqlite3 *db;

static int creat_user_tbl(void);

int database_init(void)
{
    int ret;

    ret = sqlite3_open(DATBASE_PATH, &db); //打开数据库，不存在则创建
    if (ret)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stderr, "Opened database successfully\n");
        ret = creat_user_tbl();
    }
    
    return ret;
}

/*
  * @brief:         创建表    
  * @param[in]:     None
  * @retval[out]:   None
  * @note:          表的三个字段分别是用户客户端的soket fd 、 用户名、用户密码     
  * @author:        guangjieMVP
  * @github:        https://github.com/guangjieMVP
*/
static int creat_user_tbl(void)
{
    int ret;
    char **err_msg;

    char sql[512] = {0};
    sprintf(sql, "create table if not exists %s(fd integetr, name char, password char);", TABLE_NAME);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stdout, "Table created successfully\n");
    }
    return ret;
}



static uint8_t user_is_exist = 0;

static int usernamecallback(void *NotUsed, int column_Count, char **column_Val, char **column_name)
{
    user_is_exist++;
    return 0;
} 

/*
  * @brief:         检查用户名是否已经存在    
  * @param[in]:     None
  * @retval[out]:   None
  * @note:              
  * @author:        guangjieMVP
  * @github:        https://github.com/guangjieMVP
*/
int check_is_username_exist(const char *username)
{
    int ret;

    char sql[512] = {0};
    user_is_exist = 0;

    sprintf(sql, "select * from %s where name =\"%s\";", TABLE_NAME, username);

    ret = sqlite3_exec(db, sql, usernamecallback, 0, NULL);

    return user_is_exist; 
}

static uint8_t user_is_right = 0;
static int usercallback(void *NotUsed, int column_Count, char **column_Val, char **column_name)
{
    user_is_right++;
    return 0;
} 
/*
  * @brief:         检查用户是否存在  
  * @param[in]:     None
  * @retval[out]:   None
  * @note:              
  * @author:        guangjieMVP
  * @github:        https://github.com/guangjieMVP
*/
int check_username_and_password_exist(char *name, char *password)
{
    int ret;
    char sql[512] = {0};
    user_is_right = 0;

    sprintf(sql, "select * from %s where name = \"%s\" and password=\"%s\";", TABLE_NAME, name, password);

    ret = sqlite3_exec(db, sql, usercallback, 0, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stdout, "select successfully\n");
    }

    return user_is_right;
}


int set_user_fd_field_by_name(int fd, char *username)
{
    int ret;
    char sql[512] = {0};

    sprintf(sql, "update %s set fd=%d where name=\"%s\";", TABLE_NAME, fd, username);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
    return (ret != SQLITE_OK ? 0 : 1);
}


int set_user_password_field_by_name(char *password, char *username)
{
    int ret;
    char sql[512] = {0};

    sprintf(sql, "update %s set password=\"%s\" where name=\"%s\";", TABLE_NAME, password, username);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);

    return (ret != SQLITE_OK ? 0 : 1);
}

int set_name_field_by_fd(char *username, int fd)
{
    int ret;
    char sql[512] = {0};

    sprintf(sql, "update %s set name=\"%s\" where fd=%d;", TABLE_NAME, username, fd);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);

    return (ret != SQLITE_OK ? 0 : 1);
}

int set_password_field_by_fd(char *password, int fd)
{
    int ret;
    char sql[512] = {0};

    sprintf(sql, "update %s set password=\"%s\" where fd=%d;", TABLE_NAME, password, fd);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);

    return (ret != SQLITE_OK ? 0 : 1);
}

int show_online_user(void)
{
    int ret;

    char sql[512] = {0};
    sprintf(sql, "select * from %s", TABLE_NAME);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stdout, "select successfully\n");
    }
    return ret;
}


static int callback(void *NotUsed, int column_Count, char **column_Val, char **column_name)
{
    int i;
//    printf("argc %d\r\n", column_Count);
    for (i = 0; i < (column_Count); i++)
    {
        printf("  %s : %s  ", column_name[i], column_Val[i] ? column_Val[i] : "NULL");
    }
    printf("\n");
    return 0;
} 

int show_all_user(void)
{
    int ret;

    char sql[512] = {0};
    sprintf(sql, "select * from %s", TABLE_NAME);

    ret = sqlite3_exec(db, sql, callback, 0, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stdout, "select successfully\n");
    }
    return ret;
}

/*
  * @brief:         向用户表中插入字段    
  * @param[in]:     None
  * @retval[out]:   None
  * @note:              
  * @author:        guangjieMVP
  * @github:        https://github.com/guangjieMVP
*/
int insert_user_tbl_for_register(const int fd, const char *username, const char *password)
{
    int ret;
    char **err_msg;

    char sql[512] = {0};

    ret = check_is_username_exist(username);
//    printf("check username %d\n", ret);
    if (ret) 
        return -1;    

    sprintf(sql, "insert into %s values(%d, \"%s\", \"%s\");", TABLE_NAME, fd, username, password);
          //执行sql语句
    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stdout, "insert successfully\n");
    }
    return ret;
}

int delete_tbl(char *tblname)
{
    int ret;

    char sql[512] = {0};
    sprintf(sql, "delete from %s", tblname);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        fprintf(stdout, "delete table successfully\n");
    }
    return ret;
}

#define TEST_DEBUG  1

#if TEST_DEBUG   

int main(void)
{
    database_init();

    insert_user_tbl_for_register(-1, "ares", "12345");
    insert_user_tbl_for_register(-1, "sola", "3455");
    insert_user_tbl_for_register(-1, "yuying", "1265"); 

//    check_is_username_exist("ares");
    int ret = check_username_and_password_exist("ares", "12345");  //用户存在
    printf("ret = %d\n", ret);
    if (ret != 0)
    {
        int fd = 520;
        ret = set_user_fd_field_by_name(fd, "ares");           //设置用户字段
        printf("set_user_fd : %d\n", ret);
    }

    ret = set_user_password_field_by_name("2333", "sola");
    printf("set_user_password : %d\n", ret);

    ret = set_name_field_by_fd("doge", 520);
    printf("set_name_field : %d\n", ret);

    ret = set_password_field_by_fd("11054748293", 520);
    printf("set_name_field : %d\n", ret);

    ret = show_all_user();

    delete_tbl(TABLE_NAME);
//     
}

#endif





