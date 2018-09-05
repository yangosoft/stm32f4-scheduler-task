#ifndef __TASK_H
#define __TASK_H

#include <stdint.h>

#define TASK_MAX 4

#define TASK_FREE 	0
#define TASK_READY 	1
#define TASK_RUNNING 	2
#define QUANTUM 2

/* Need to mask out thumb bit */
#define TASK_PC_MASK 0xfffffffe

#define MAIN_SP 	0x20001000
#define PROC_STACK_SIZE 256
#define PROC_STACK_TOP 	(MAIN_SP - PROC_STACK_SIZE)

#define PSR_INIT 0x21000000 

#define EXC_RET_THREAD 0xfffffff9


struct task_t {
	uint32_t *sp;
	uint32_t state;
        uint8_t quantum;
};

void task_init();
int task_create(void (*task)(void));
struct task_t * task_ready();
__attribute__((naked)) void task_switch(void);
#endif /* __TASK_H */
