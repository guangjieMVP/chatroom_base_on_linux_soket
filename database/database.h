#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <sqlite3.h>

int database_init(void);

int insert_usertbl_all_field(const int fd, const char *username, const char *password);

int check_is_username_exist(const char *username);
int check_username_and_password_exist(char *name, char *password);

int set_user_fd_field_by_name(int fd, char *username);
int set_user_password_field_by_name(char *password, char *username);
int set_name_field_by_fd(char *username, int fd);
int set_password_field_by_fd(char *password, int fd);

int show_all_users(sqlite3_callback callback);
int show_online_users(sqlite3_callback callback);   

int get_register_user_num(void);
int get_online_user(sqlite3_callback callback, int fd);

#endif