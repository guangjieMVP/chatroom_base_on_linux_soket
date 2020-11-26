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

#define RETURN_VAL(val)  (val != SQLITE_OK ? 0 : 1)

#define USE_SQL_LOG       1

static sqlite3 *db;

static int creat_user_tbl(void);


#if USE_SQL_LOG
static void print_sql_op_ret_log(const int ret, const char *op, const char *err)
{
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "%s SQL error: %s\n", sqlite3_errmsg(db), err); 
    }
    else
    {
        fprintf(stdout, "%s successfully\n", op);
    }
}
#endif

/*
  * @brief:             
  * @param[in]:     None
  * @retval[out]:   1 - 初始化成功
  * @note:              
  * @author:        guangjieMVP
  * @github:        https://github.com/guangjieMVP
*/
int database_init(void)
{
    int ret;

    ret = sqlite3_open(DATBASE_PATH, &db); //打开数据库，不存在则创建
#
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
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "Table created", "create user tbl");
#endif 
    return RETURN_VAL(ret);
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
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "select username", "check username");
#endif
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
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "check usename and password", "check usename and password");
#endif
    return user_is_right;
}


int set_user_fd_field_by_name(int fd, char *username)
{
    int ret;
    char sql[512] = {0};

    sprintf(sql, "update %s set fd=%d where name=\"%s\";", TABLE_NAME, fd, username);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "upate set fd", "upate set fd");
#endif
    return RETURN_VAL(ret);
}


int set_user_fd_field_by_fd(int fd, int socketfd)
{
    int ret;
    char sql[512] = {0};

    sprintf(sql, "update %s set fd=%d where fd=%d;", TABLE_NAME, fd, socketfd);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "upate set fd", "upate set fd");
#endif
    return RETURN_VAL(ret);
}


int set_user_password_field_by_name(char *password, char *username)
{
    int ret;
    char sql[512] = {0};

    sprintf(sql, "update %s set password=\"%s\" where name=\"%s\";", TABLE_NAME, password, username);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "upate set password", "upate set password");
#endif
    return RETURN_VAL(ret);
}

int set_name_field_by_fd(char *username, int fd)
{
    int ret;
    char sql[512] = {0};

    sprintf(sql, "update %s set name=\"%s\" where fd=%d;", TABLE_NAME, username, fd);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "set name by fd", "set name by fd");
#endif
    return RETURN_VAL(ret);
}

int set_password_field_by_fd(char *password, int fd)
{
    int ret;
    char sql[512] = {0};

    sprintf(sql, "update %s set password=\"%s\" where fd=%d;", TABLE_NAME, password, fd);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "set password by fd", "set password by fd");
#endif
    return RETURN_VAL(ret);
}


int get_online_user(sqlite3_callback callback, int fd)
{
    int ret;

    char sql[512] = {0};
    sprintf(sql, "select * from %s where fd > 0 and fd != %d", TABLE_NAME, fd);

    ret = sqlite3_exec(db, sql, callback, 0, NULL);
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "get_online_user", "get_online_user");
#endif
    return RETURN_VAL(ret);
}




static int _callback(void *NotUsed, int column_Count, char **column_Val, char **column_name)
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

int show_online_users(sqlite3_callback callback)
{
    int ret;

    char sql[512] = {0};
    sprintf(sql, "select * from %s where fd > 0", TABLE_NAME);

    ret = sqlite3_exec(db, sql, callback, 0, NULL);
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "show all users", "show all users");
#endif
    return RETURN_VAL(ret);
}


int show_all_users(sqlite3_callback callback)
{
    int ret;

    char sql[512] = {0};
    sprintf(sql, "select * from %s", TABLE_NAME);

    ret = sqlite3_exec(db, sql, callback, 0, NULL);
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "show all users", "show all users");
#endif
    return RETURN_VAL(ret);
}

static int user_num = 0;
static int _user_num_callback(void *NotUsed, int column_Count, char **column_Val, char **column_name)
{
    int i;

    user_num++;
    return 0;
} 

//计算注册的用户的数量
int get_register_user_num(void)
{
    int ret;
    user_num = 0;

    char sql[512] = {0};
    sprintf(sql, "select * from %s", TABLE_NAME);

    ret = sqlite3_exec(db, sql, _user_num_callback, 0, NULL);
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "show all users", "show all users");
#endif
    return user_num;
}



/*
  * @brief:         向用户表中插入字段    
  * @param[in]:     None
  * @retval[out]:   None
  * @note:              
  * @author:        guangjieMVP
  * @github:        https://github.com/guangjieMVP
*/
int insert_usertbl_all_field(const int fd, const char *username, const char *password)
{
    int ret;
    char **err_msg;

    char sql[512] = {0};

    sprintf(sql, "insert into %s values(%d, \"%s\", \"%s\");", TABLE_NAME, fd, username, password);
          //执行sql语句
    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "insert usertbl all", "insert usertbl all");
#endif
    return RETURN_VAL(ret);
}

/*
  * @brief:             
  * @param[in]:     None
  * @retval[out]:   None
  * @note:              
  * @author:        guangjieMVP
  * @github:        https://github.com/guangjieMVP
*/
int delete_tbl(char *tblname)
{
    int ret;
    char *op = "delete";

    char sql[512] = {0};
    sprintf(sql, "delete from %s", tblname);

    ret = sqlite3_exec(db, sql, NULL, 0, NULL);
#if USE_SQL_LOG
    print_sql_op_ret_log(ret, "delete table", "delete table");
#endif

    return RETURN_VAL(ret);
}

#define TEST_DEBUG  0

#if TEST_DEBUG   

int main(void)
{
    database_init();

    if (!check_is_username_exist("ares")) 
    {
        insert_usertbl_all_field(-1, "ares", "12345");
    }
    else
    {
        printf("ares already exists\n");
    }
    
    if (!check_is_username_exist("sola")) 
    {
        insert_usertbl_all_field(-1, "sola", "3455"); 
    }
    else
    {
        printf("sola already exists\n");
    }
    
    if (!check_is_username_exist("yuying")) 
    {
        insert_usertbl_all_field(-1, "yuying", "1265");  
    }
    else
    {
        printf("yuying already exists\n");
    }
    
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

    ret = set_user_fd_field_by_fd(-10, 520);
    printf("set_user_fd_field_by_fd : %d\n", ret);

    ret = show_all_users(_callback);

    delete_tbl(TABLE_NAME);
//     
}

#endif





