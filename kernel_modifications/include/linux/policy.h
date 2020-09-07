#ifndef _OUR_POLICY_H_
#define _OUR_POLICY_H_

#include <linux/types.h>
#include <linux/sched.h>
//for kfree
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/param.h>
#include <linux/signal.h>


void update_task_policy_info(struct task_struct * pTask);
void after_sleep(unsigned long data);
void after_terminate(unsigned long data);
void our_timeout(struct task_struct * pTask);
void run_policies(struct task_struct * pTask);

int sys_set_policy(pid_t pid, int policy_id, int policy_value);
int sys_get_policy(pid_t pid, int* policy_id, int* policy_value);

#endif
