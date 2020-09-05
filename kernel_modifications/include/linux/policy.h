#ifndef _OUR_POLICY_H_
#define _OUR_POLICY_H_

#include <linux/types.h>
#include <linux/sched.h>
//for kfree
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/list.h>
#include <linux/errno.h> 

typedef struct policy_inst
{
    struct list_head list_pointers;
    int policy_id;
    int policy_value;
}policy_inst, *p_policy_inst;

int sys_set_policy(pid_t pid, int policy_id, int policy_value);
int sys_get_policy(pid_t pid, int* policy_id, int* policy_value);


#endif
