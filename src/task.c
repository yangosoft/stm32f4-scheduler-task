#include <stdint.h>
#include <task.h>



static struct task_t task_table[TASK_MAX] __attribute__ ((section(".data")));

/* Number of tasks created */
static int task_created __attribute__ ((section(".data")));


static int task_index __attribute__ ((section(".data")));

int i __attribute__ ((section(".data")));

void task_init() {
    i = 0;
    task_index = 0;
    for (i = 0; i < TASK_MAX; i++) {
        task_table[i].state = TASK_FREE;
    }
}

int task_create(void (*task)(void)) {
    i = 0;

    for (i = 0; i < TASK_MAX; i++) {
        if (task_table[i].state == TASK_FREE) {
            task_table[i].sp = (void *) (PROC_STACK_TOP - (i * PROC_STACK_SIZE));

            /* setup initial stack frame */
            *(task_table[i].sp--) = PSR_INIT;
            *(task_table[i].sp--) = (uint32_t) task & TASK_PC_MASK;
            *(task_table[i].sp--) = 0; /* lr */
            *(task_table[i].sp--) = 0; /* r12 */
            *(task_table[i].sp--) = 0; /* r3  */
            *(task_table[i].sp--) = 0; /* r2  */
            *(task_table[i].sp--) = 0; /* r1  */
            *(task_table[i].sp--) = 0; /* r0  */

            *(task_table[i].sp--) = 0; /* r11  */
            *(task_table[i].sp--) = 0; /* r10  */
            *(task_table[i].sp--) = 0; /* r9   */
            *(task_table[i].sp--) = 0; /* r8   */
            *(task_table[i].sp--) = 0; /* r7   */
            *(task_table[i].sp--) = 0; /* r6   */
            *(task_table[i].sp--) = 0; /* r5   */
            *(task_table[i].sp) = 0; /* r4   */

            task_table[i].state = TASK_READY;
            task_table[i].quantum = 0;

            task_created++;
            return 0;
        }
    }

    return -1;
}

struct task_t * task_ready() {

    for (task_index = 0; task_index < TASK_MAX; task_index++) {
        puts("Task ");
        console_putc('0' + task_index);
        puts(" status ");
        console_putc('0' + task_table[task_index].state);
        puts("\n");

        if ((task_table[task_index].state == TASK_READY) && (task_table[task_index].quantum < QUANTUM)) {
            task_table[task_index].quantum++;
            return &(task_table[task_index]);
        }
    }


    for (task_index = 0; task_index < TASK_MAX; task_index++) {
        if ((task_table[task_index].state == TASK_READY) && (task_table[task_index].quantum >= QUANTUM)) {
            task_table[task_index].quantum = 0;
        }
    }




    return (struct task_t *) 0;
}

struct task_t * task_running() {
    i = 0;

    for (i = 0; i < TASK_MAX; i++) {
        if (task_table[i].state == TASK_RUNNING) {
            return &(task_table[i]);
        }
    }

    return (struct task_t *) 0;
}

void *saved_psp __attribute__ ((section(".data")));

__attribute__ ((naked))void task_save_psp(void *sp) {
    saved_psp = sp;
    asm ("bx lr");
}

void * task_get_psp(void) {
    return saved_psp;
}

__attribute__ ((naked))void task_save(void *task_sp) {
    /* Save MSP */
    asm ("mov r1, sp");
    /* Copy registers */
    asm ("mov sp, r0");
    asm ("push {r4-r11}");

    /* Save new PSP */
    asm ("mov r0, sp");

    /* Restore MSP */
    asm ("mov sp, r1");

    /* Save lr */
    asm ("push {lr}");

    /* Really save psp */
    asm ("bl task_save_psp");

    struct task_t *task_cur = (struct task_t *) 0;

    task_cur = task_running();

    if (task_cur) {
        /* Don't set this here */
        /* task_cur->state = TASK_READY; */
        task_cur->sp = task_get_psp();
    } else {
        /* We are in MSP mode already */
    }

    asm ("pop {lr}");
    asm ("bx lr");
}

__attribute__ ((naked)) void task_restore(void *task_sp) {
    /* Save MSP */
    asm ("mov r1, sp");
    asm ("msr msp, r1");

    /* Restore task stackpointer */
    asm ("mov sp, r0");

    /* Restore sw frame */
    asm ("pop {r4-r11}");

    /* Restore psp */
    asm ("mov r0, sp");
    asm ("msr psp, r0");

    /* MSP back to sp */
    asm ("mov sp, r1");

    /* Return from exception */
    asm ("mov lr, #0xfffffffd");
    asm ("bx lr");
}

__attribute__ ((naked)) void task_switch() {
    puts("task_switch\n");
    /* Get PSP */
    asm("mrs r0, psp");

    /* Is it non zero */
    /* then save state */
    asm("cmp r0, #0");
    asm("it ne");
    asm("blne task_save");

    struct task_t *task_cur = (struct task_t *) 0;
    struct task_t *task_next = (struct task_t *) 0;

    task_cur = task_running();
    task_next = task_ready();

    if (task_next && task_cur) {
        task_cur->state = TASK_READY;
    }

    if (task_next) {
        //                 puts("We have next task\n");
        task_next->state = TASK_RUNNING;
        task_restore(task_next->sp);
        /* Never return from here */
    }

    puts("No task :(\n");
    /* Return from exception */
    asm("mov lr, #0xfffffff9");
    asm("bx lr");
}
