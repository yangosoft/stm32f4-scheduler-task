.syntax unified

.cpu cortex-m4
.thumb

/* Vector Table */

.word 0x20001000        /* Stack Pointer */
.word reset_handler	/* 1 Reset Handler */
.word default_handler   /* 2 NMI */
.word default_handler   /* 3 Hard Fault */
.word default_handler   /* 4 Mem Management Fault */
.word default_handler   /* 5 Bus Fault */
.word usage_handler     /* 6 Usage Fault */
.word default_handler   /* 7 RESERVED */
.word default_handler   /* 8 RESERVED */
.word default_handler   /* 9 RESERVED*/
.word default_handler   /* 10 RESERVED */
.word default_handler   /* 11 SVCall */
.word default_handler   /* 12 Debug Monitor */
.word default_handler   /* 13 RESERVED */
.word default_handler   /* 14 PendSV */
.word timer_func   /* 15 SysTick */

.global reset_handler
.type reset_handler, STT_FUNC
.thumb_func
reset_handler:
	bl main
halt:
	bl halt

.global default_handler
.type default_handler, STT_FUNC
.thumb_func
default_handler:
	bl default_handler

.global usage_handler
.type usage_handler, STT_FUNC
.thumb_func
usage_handler:
	bl usage_handler
