#ifndef _OUR_POLICY_H_
#define _OUR_POLICY_H_

#include <linux/types.h>
#include <linux/sched.h>
//for kfree
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/list.h>
#include <linux/errno.h> 

int sys_set_policy(pid_t pid, int policy_id, int policy_value);
int sys_get_policy(pid_t pid, int* policy_id, int* policy_value);

#endif
