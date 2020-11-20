#include "common.h"

void cmd_handle(struct cmd *cmds, uint32_t len, int cmd)
{
    int index;

    for (index = 0; index < len; index++) 
    {
        if (cmds[index].cmd == cmd)
        {
            if (cmds[index].cmd_callback)
            {
                cmds[index].cmd_callback();
            }
            break;
        }
    }
}