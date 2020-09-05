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
///*************************remove policy_inst*************************************//
void remove(p_policy_inst curr ){
    printk("In remove\n"); //DEBUG
    if (curr == NULL)
        return;

    //delete list_head struct
    list_del(&curr->list_pointers); //?maybe I need to delete curr instead//
    kfree(curr);
    printk("\tpassed  kfree\n"); //DEBUG
}

///***************************policy_sleep*****************************************//
void run_policies(struct task_struct * pTask){

    struct  list_head* curr;
    p_policy_inst curr_policy_inst;
    p_policy_inst next_policy_inst;
    while(!list_empty(pTask->policy_stack_head)){
        //oldest policy
        curr_policy_inst = list_entry(pTast->policy_stack_head.prev,policy_inst,list_pointers);
        if(curr_policy_inst->policy_id == 1){ //sleep
            pTask->state = TASK_UNINTERRUPTIBLE;
            schedule_timeout((signed long)policy_value);
            if(curr_policy_inst->list_pointers.next == pTask->policy_stack_head){ //no more polices
                wake_up_process(pTask->pid);
            }
            else{
                next_policy_inst = list_entry(curr_policy_inst->list_pointers.next,policy_inst,list_pointers);
                if(next_policy_inst->policy_id == 0){
                    wake_up_process(pTask->pid);
                }
            }
        }else if(curr_policy_inst->policy_id == 2){//terminate
            pTask->state = TASK_UNINTERRUPTIBLE;
            schedule_timeout((signed long)policy_value);
            if(curr_policy_inst->list_pointers.next == pTask->policy_stack_head) { //no more policies
                //terminate
            }
            else if {
                next_policy_inst = list_entry(curr_policy_inst->list_pointers.next, policy_inst, list_pointers);
                if (next_policy_inst->policy_id == 2) { //also terminate
                    //terminate
                }
            }
        }
        remove(curr_policy_inst);
    }
}

////******************************set policy**************************************************************************/
int sys_set_policy(pid_t pid, int policy_id, int policy_value){
    int was_list_empty;
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
	p_policy_inst  new_policy_inst = kmalloc(sizeof(TODO_inst),GFP_KERNEL);
	if(new_policy_inst == NULL){
        printk("\tallocation of new_policy_inst failed\n"); //DEBUG
        return -ENOMEM;
	}
	was_list_empty = list_empty(pTask->policy_stack_head);
	new_policy_inst->policy_id = policy_id;
    new_policy_inst->policy_value = policy_value;
    list_add(&new_policy_inst->list_pointers,&pTask->policy_stack_head);

    //if list was empty we need to start the policies.
    // if list wasn't empty we don't need to start the policy
    // policy will start after previous policies happned
    if(was_list_empty){
        run_policies(pTask);
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

    p_policy_inst oldest_policy_inst = list_entry(pTast->policy_stack_head.prev,policy_inst,list_pointers);
    result = copy_to_user(policy_id,&oldest_policy_inst->policy_id, sizeof(int));
    if(result != 0){
        printk("\tcopy_to_user of policy_id did not fully succeed\n"); //DEBUG
        // error copying to user space
        return -EFAULT;
    }
    result2 = copy_to_user(policy_value,&oldest_policy_inst->policy_value, sizeof(int));
    if(result2 != 0){
        printk("\tcopy_to_user of policy_value did not fully succeed\n"); //DEBUG
        // error copying to user space
        return -EFAULT;
    }
	// on success
	return 0;
}