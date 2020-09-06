#include <linux/policy.h>
#include <linux/delay.h>
#include <linux/time.h> //DEBUG
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
///***************************our_timeout******************************************//
void our_timeout(struct task_struct * pTask){
    printk("\tpid of task that is going to do policy =%d\n",pTask->pid); //DEBUG

    struct timeval time;
    struct timespec timeout_secs;
    timeout_secs.tv_sec = pTask->policy_value;
    timeout_secs.tv_nsec = 0;

    signed long timeout_jiffies = timespec_to_jiffies(&timeout_secs);

    printk("\tpolicy_value =%d\n",pTask->policy_value); //DEBUG
    printk("\ttimeout_jiffies=%lu\n",timeout_jiffies); //DEBUG
    do_gettimeofday(&time);
    printk("\tbefore time_out time =%d\n",time.tv_sec); //DEBUG

    schedule_timeout(timeout_jiffies);

    do_gettimeofday(&time);
    printk("\tafter time_out time =%d\n",(int)time.tv_sec); //DEBUG
}

//******************************after sleep***************************************//
void after_sleep(struct task_struct * pTask){
    if(pTask->changed_policy == 0){ //no more polices
        printk("\tno more polices -> wake_up_process \n"); //DEBUG
        wake_up_process(pTask);
    }
    else{
        if(pTask->next_policy_id == 0){
            printk("\tnext policy is 0 -> wake_up_process\n"); //DEBUG
            wake_up_process(pTask);
        }
    }
}

//****************************after_terminate**************************************//
void after_terminate(struct task_struct * pTask){
    if(pTask->changed_policy == 0){ //no more polices
        printk("\tno more polices -> do_exit\n"); //DEBUG
        do_exit(code);
        return;
    }
    else{
        if (pTask->next_policy_id == 2) { //next inst is also terminate
            printk("\tnext policy is terminate -> do_exit\n"); //DEBUG
            do_exit(code);
            return;
        }
    }
}
///***************************policy_sleep*****************************************//
void run_policies(struct task_struct * pTask){
    printk("run_policies\n"); //DEBUG
    long code;

    pTask->started_policy = 1;
    do{
        printk("\tinside do_while\n"); //DEBUG
        if(pTask->policy_id == 1){ //sleep
            printk("\tpolicy: sleep\n"); //DEBUG

            set_current_state(TASK_UNINTERRUPTIBLE);
            printk("\tchanged state TASK_UNINTERRUPTIBLE\n"); //DEBUG
            our_timeout(pTask);
            after_sleep(pTask);

        }else if(pTask->policy_id == 2){//terminate
            printk("\tpolicy: terminate\n"); //DEBUG

            our_timeout(pTask);
            after_termiate(pTask);
        }
        if(pTask->changed_policy ==1){
            printk("\tchanged_policy ==1 -> change values\n"); //DEBUG
            pTask->policy_id = pTask->next_policy_id;
            pTask->policy_value = pTask->next_policy_value;
            pTask->next_policy_id  = -1;
            pTask->next_policy_value = 0;
            pTask->changed_policy = 0;
        }
    }while(pTask->changed_policy == 1);

    printk("\tfinished with policies start_policy=0\n"); //DEBUG
    pTask->started_policy = 0;

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

    //if pTask->started_policy==0 we need to start the policies.
    // if pTask->started_policy==1 we don't need to start the policy
    // policy will start after previous policies happened
    if(!pTask->started_policy){
        pTask->policy_id = policy_id;
        pTask->policy_value = policy_value;
        run_policies(pTask);
    }else{
        pTask->next_policy_id  = policy_id;
        pTask->next_policy_value = policy_value;
        pTask->changed_policy = 1;
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

    int result1 = copy_to_user(policy_id,&pTask->policy_id, sizeof(int));
    if(result1 != 0){
        printk("\tcopy_to_user of policy_id did not fully succeed\n"); //DEBUG
        // error copying to user space
        return -EFAULT;
    }
    int result2 = copy_to_user(policy_value,&pTask->policy_value, sizeof(int));
    if(result2 != 0){
        printk("\tcopy_to_user of policy_value did not fully succeed\n"); //DEBUG
        // error copying to user space
        return -EFAULT;
    }
	// on success
	return 0;
}

/**TIMER**/
void change(int sec){
    struct timer_list timer;
    unsigned long expire = sec + jiffies; //TODO: sec to jiffs

    init_timer(&timer);
    timer.expires = expire;
    timer.data = (unsigned long) current; //TODO: what our func needs
    timer.function = process_timeout; //TODO: our func

    add_timer(&timer);
    schedule();
    del_timer_sync(&timer);
}