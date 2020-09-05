#include <linux/policy.h>

// *******************return value: 0 if allowed, -1 if not allowed**************************************************/
int check_if_allowed(pid_t curr, pid_t add_to){
    printk("In check_if_allowed\n"); //DEBUG
    if (curr == add_to)
        return 0;
    struct task_struct * parent_task = find_task_by_pid(add_to);
    if (parent_task == NULL)
        return -1;
    // while still has ancestors
    while(parent_task != NULL){
        printk("\tinside while\n"); //DEBUG
        if (parent_task->pid == 1)
            return -1;
        else if (parent_task->pid == curr)
            return 0;
        parent_task = parent_task->p_pptr;
    }
    //shouldn't get here
    return -1;
}

///***************************policy_sleep*****************************************//
void policy_sleep(pid_t pid, int policy_value,struct task_struct * pTask){

    if(pTask->state ==  TASK_UNINTERRUPTIBLE){
        return
    }
    else{
        while(pTask->policy_changed){
            pTask->policy_changed = 0;
            pTask->state = TASK_UNINTERRUPTIBLE;
            schedule_timeout((signed long)policy_value);
        }
        wake_up_process(pid);
    }
}

////******************************set policy**************************************************************************/
int sys_set_policy(pid_t pid, int policy_id, int policy_value){
	printk("In set_policy\n"); //DEBUG
	if (policy_value < 0 || policy_id < 0 || policy_id > 2)
		return -EINVAL;
	printk("\tvalid inputs\n"); //DEBUG
	struct task_struct * pTask = find_task_by_pid(pid);
	printk("\tafter find_task_by_pid\n"); //DEBUG
	//if No such process exists
	if (pTask == NULL)
		return -ESRCH;
	printk("\tprocess exists\n"); //DEBUG
	// if process not allowed to change policy
	if(check_if_allowed(current->pid,pid) == -1)
		return -ESRCH;
	printk("\tallowed to change policy\n"); //DEBUG
    pTask->policy_id = policy_id;
    pTask->policy_value = policy_value;
    pTask->policy_changed = 1;

    if(pTask->policy_id == 1){
        policy_sleep(pid,policy_value,pTak);
    }
	// on success
	return 0;
}

////******************************get policy**************************************************************************/
int sys_get_policy(pid_t pid, int* policy_id, int* policy_value){
	printk("In check_if_allowed\n"); //DEBUG
	if (policy_id == NULL || policy_value == NULL)
		return -EINVAL;
	printk("\tvalid inputs\n"); //DEBUG
	struct task_struct * pTask = find_task_by_pid(pid);
	printk("\tafter find_task_by_pid\n"); //DEBUG
	//if No such process exists
	if (pTask == NULL)
		return -ESRCH;
	printk("\tprocess exists\n"); //DEBUG
	// if process not allowed to change policy
	if(check_if_allowed(current->pid,pid) == -1)
		return -ESRCH;
	printk("\tallowed to get policy\n"); //DEBUG
    result = copy_to_user(policy_id,&pTask->policy_id, sizeof(int));
    if(result != 0){
        printk("\tcopy_to_user of policy_id did not fully succeed\n"); //DEBUG
        // error copying to user space
        return -EFAULT;
    }
    result2 = copy_to_user(policy_value,&pTask->policy_value, sizeof(int));
    if(result2 != 0){
        printk("\tcopy_to_user of policy_value did not fully succeed\n"); //DEBUG
        // error copying to user space
        return -EFAULT;
    }
	// on success
	return 0;
}