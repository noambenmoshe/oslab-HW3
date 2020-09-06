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

///***********************update_task_policy_info********************************//
void update_task_policy_info(struct task_struct * pTask){
    printk("in update_task_policy_info\n"); //DEBUG
    if(pTask->changed_policy == 1){
        printk("\tchanged_policy ==1 -> change values\n"); //DEBUG
        pTask->policy_id = pTask->next_policy_id;
        pTask->policy_value = pTask->next_policy_value;
        pTask->next_policy_id  = -1;
        pTask->next_policy_value = 0;
        pTask->changed_policy = 0;
    }
    printk("\tfinished with policies start_policy=0\n"); //DEBUG
    pTask->started_policy = 0;
}

//******************************after sleep***************************************//
void after_sleep(unsigned long data){
    struct task_struct * pTask =(task_t*)data;
    if(pTask == NULL){
        printk("\tpTask is NULL\n"); //DEBUG
        return;
    }
    int res;
    struct timeval time;
//    do_gettimeofday(&time);
//    printk("\tafter time_out time =%d\n",(int)time.tv_sec); //DEBUG

    if(pTask->changed_policy == 0){ //no more polices
        printk("\tno more polices -> wake_up_process \n"); //DEBUG
        res = wake_up_process(pTask);
        printk("\twake_up_process = %d \n",res); //DEBUG
        update_task_policy_info(pTask);
    }
    else{
        if(pTask->next_policy_id == 0){ //NO POLICY
            printk("\tnext policy is 0 -> wake_up_process\n"); //DEBUG
            wake_up_process(pTask);
            update_task_policy_info(pTask);
        } else if(pTask->next_policy_id == 1) { //SLEEP
            update_task_policy_info(pTask);
            run_policies(pTask);
        }else if(pTask->next_policy_id == 2){
            wake_up_process(pTask);
            update_task_policy_info(pTask);
            run_policies(pTask);
        }
    }
    printk("\tend after_sleep \n"); //DEBUG
}

//****************************after_terminate**************************************//
void after_terminate(unsigned long data){
    struct task_struct * pTask =(task_t*)data;
    long code;
    struct timeval time;
    do_gettimeofday(&time);
    printk("\tafter time_out time =%d\n",(int)time.tv_sec); //DEBUG

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
        }else if (pTask->next_policy_id == 1){
            update_task_policy_info(pTask);
            run_policies(pTask);
        }else if (pTask->next_policy_id == 0){
            update_task_policy_info(pTask);
        }
    }
}

///***************************our_timeout******************************************//
void our_timeout(struct task_struct * pTask){
    printk("In our_timeout");//DEBUG
    printk("\tpid of task that is going to do policy =%d\n",pTask->pid); //DEBUG
    struct timeval time;
    struct timespec timeout_secs;
    timeout_secs.tv_sec = pTask->policy_value;
    timeout_secs.tv_nsec = 0;
    signed long timeout_jiffies = timespec_to_jiffies(&timeout_secs);
    struct timer_list timer;
    unsigned long expire = timeout_jiffies + jiffies;

    printk("\tpolicy_value =%d\n",pTask->policy_value); //DEBUG
    printk("\ttimeout_jiffies=%lu\n",timeout_jiffies); //DEBUG

    init_timer(&timer);
    timer.expires = expire;
    timer.data = (unsigned long)pTask;
    if(pTask->policy_id ==  1){
        printk("\tset next function to after_sleep \n"); //DEBUG
        timer.function = after_sleep;
    }
    else if(pTask->policy_id ==  2){
        printk("\tset next function to after_terminate \n"); //DEBUG
        timer.function = after_terminate;
    }

    do_gettimeofday(&time);
    printk("\tbefore time_out time =%d\n",time.tv_sec); //DEBUG

    add_timer(&timer);
    printk("\tafter add_timer \n"); //DEBUG
    schedule();
    printk("\tafter_schedule \n"); //DEBUG
    del_timer_sync(&timer);
    printk("\tend our_timeout \n"); //DEBUG
}


///***************************run policy*****************************************//
void run_policies(struct task_struct * pTask){
    printk("run_policies\n"); //DEBUG

    pTask->started_policy = 1;
    if(pTask->policy_id == 0){
        pTask->started_policy = 0;
    }
    else if(pTask->policy_id == 1){ //sleep
        printk("\tpolicy: sleep\n"); //DEBUG
        set_current_state(TASK_UNINTERRUPTIBLE);
        printk("\tchanged state TASK_UNINTERRUPTIBLE\n"); //DEBUG
        our_timeout(pTask);

    }else if(pTask->policy_id == 2){//terminate
        printk("\tpolicy: terminate\n"); //DEBUG
        our_timeout(pTask);
    }

    printk("\tshouldn't get here\n"); //DEBUG
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
    printk("\tend sys_set\n"); //DEBUG
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

