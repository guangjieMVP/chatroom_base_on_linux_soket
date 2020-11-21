#ifndef _MENU_OPTION_H_
#define _MENU_OPTION_H_

#include "common.h"

struct option {
    int op;
    int (*op_callback)(void);
};

extern struct cmd cmd_reply_list[];
extern int cmd_reply_list_size;

extern struct option main_menu_ops[];
extern int main_menu_ops_size;

void menu_init(void);
void login_menu(void);    
int menu_option_handle(struct option *ops, uint32_t len, int op);
int input_menu_option(void);

#endif /* _MENU_OPTION_H_ */