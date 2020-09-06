#ifndef _OUR_POLICY_H_
#define _OUR_POLICY_H_

#include <linux/types.h>
#include <linux/sched.h>
//for kfree
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/list.h>
#include <linux/errno.h> 

void update_task_policy_info(struct task_struct * pTask);
void after_sleep(struct task_struct * pTask);
void after_terminate(struct task_struct * pTask);
void our_timeout(struct task_struct * pTask);
void run_policies(struct task_struct * pTask);

int sys_set_policy(pid_t pid, int policy_id, int policy_value);
int sys_get_policy(pid_t pid, int* policy_id, int* policy_value);

#endif
