/*
 * @file:        xxx.c
 * @author:      guangjieMVP
 * @version:     v1.00.00
 * @date:        2020-xx-xx
 * @github:      https://github.com/guangjieMVP
 * @brief: 
*/
#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <sqlite3.h>

int database_init(void);

int insert_user_tbl_for_register(int fd, char *username, char *password);

int check_is_username_exist(char *username);
int check_username_and_password_exist(char *name, char *password);

int set_user_fd_field_by_name(int fd, char *username);
int set_user_password_field_by_name(char *password, char *username);
int set_name_field_by_fd(char *username, int fd);
int set_password_field_by_fd(char *password, int fd);

#endif