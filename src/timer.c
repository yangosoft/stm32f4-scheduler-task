#include <stdint.h>
#include <task.h>

#define SCS_BASE            (0xE000E000UL)                            /*!< System Control Space Base Address  */
#define ITM_BASE            (0xE0000000UL)                            /*!< ITM Base Address                   */
#define CoreDebug_BASE      (0xE000EDF0UL)                            /*!< Core Debug Base Address            */

#define SYSTICK_BASE ((uint32_t *) 0xe000e000)
#define SYSTICK_CTRL ((uint32_t *) 0xe000e010)
#define SYSTICK_TRELOAD ((uint32_t *) 0xe000e014)
#define SYSTICK_CTRL_ACTIVATE (0x7)

void timer_start() {
    *SYSTICK_CTRL = SYSTICK_CTRL_ACTIVATE;
}

void timer_stop() {
    *SYSTICK_CTRL = 0x0;
}

void timer_reload(uint32_t reload) {
    *SYSTICK_TRELOAD = reload;
}

__attribute__ ((naked)) void timer_func() {

    /*
     *     // Trigger the pendSV 
     *((uint32_t volatile *)0xE000ED04) = 0x10000000;
     */
    asm("b task_switch");
}
