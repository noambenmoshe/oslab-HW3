#ifndef _POLICY_API_H
#define _POLICY_API_H

//#include <sys/types.h>
//#include <linux/stddef.h>
#include <errno.h>

int set_policy(pid_t pid, int policy_id, int policy_value)
{
    int res;
    __asm__
	(
    "pushl %%eax;"
    "pushl %%ebx;"
    "pushl %%ecx;"
    "pushl %%edx;"
    "movl $244, %%eax;"
    "movl %1, %%ebx;"
    "movl %2, %%ecx;"
    "movl %3, %%edx;"
    "int $0x80;"
    "movl %%eax,%0;"
    "popl %%edx;"
    "popl %%ecx;"
    "popl %%ebx;"
    "popl %%eax;"
    : "=m" (res)
    : "m" (pid) ,"m" (policy_id) ,"m"(policy_value)
    );
    if (res >= (unsigned long)(-125))
    {
        errno = -res;
        res = -1;
    }
    return (int) res;
}

ssize_t  get_policy(pid_t pid,  int* policy_id, int* policy_value)
{
    int res;
    __asm__
	(
    "pushl %%eax;"
    "pushl %%ebx;"
    "pushl %%ecx;"
    "pushl %%edx;"
    "movl $243, %%eax;"
    "movl %1, %%ebx;"
    "movl %2, %%ecx;"
    "movl %3, %%edx;"
    "int $0x80;"
    "movl %%eax,%0;"
    "popl %%edx;"
    "popl %%ecx;"
    "popl %%ebx;"
    "popl %%eax;"
    : "=m" (res)
    : "m" (pid) ,"m" (policy_id) ,"m"(policy_value)
    );
    if (res >= (unsigned long)(-125))
    {
        errno = -res;
        res = -1;
    }
    return (int) res;
}


#endif
