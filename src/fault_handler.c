#include <stdint.h>
#include <stdio.h>

void dump_registers(uint32_t stack[]);
void base_fault_handler(uint32_t stack[]);
void hard_fault_handler(void);

// Core ARM interrupt names. These interrupts are the same across the family.
static const char *system_interrupt_names[16] = {
    "SP_Main",      "Reset",    "NMI",        "Hard Fault",
    "MemManage",    "BusFault", "UsageFault", "Reserved",
    "Reserved",     "Reserved", "Reserved",   "SVCall",
    "DebugMonitor", "Reserved", "PendSV",     "SysTick"};

enum { r0, r1, r2, r3, r12, lr, pc, psr };


// Interrupt control and state register (RW)
#define ICSR 0xE000ED04
// Configurable Fault Status Register
#define CFSR 0xE000ED28
// MemManage Fault Address Register
#define MMFAR 0xE000ED34

//// Register subfields
// CFSR
// 1 if MMFAR has valid contents
#define CFSR_MMFSR_MMARVALID (1 << 7)
// 1 if fault occurred during FP lazy state preservation
#define CFSR_MMFSR_MLSPERR (1 << 5)
// 1 if fault occurred on exception entry
#define CFSR_MMFSR_MSTKERR (1 << 4)
// 1 if fauly occurred on exception return
#define CFSR_MMFSR_MUNSTKERR (1 << 3)
// 1 if a data access violation occurred
#define CFSR_MMFSR_DACCVIOL (1 << 1)
// 1 if an eXecute Never violation has occurred
#define CFSR_MMFSR_IACCVIOL (1 << 0)


#warning Needs an sprintf implementation
#warning Replace fault_handler_printf with a custom function

#define fault_handler_printf puts
#define sprintf


void dump_registers(uint32_t stack[]) {
  static char msg[32];
  sprintf(msg, "r0  = 0x%08x\n", stack[r0]);
  fault_handler_printf(msg);
  sprintf(msg, "r1  = 0x%08x\n", stack[r1]);
  fault_handler_printf(msg);
  sprintf(msg, "r2  = 0x%08x\n", stack[r2]);
  fault_handler_printf(msg);
  sprintf(msg, "r3  = 0x%08x\n", stack[r3]);
  fault_handler_printf(msg);
  sprintf(msg, "r12 = 0x%08x\n", stack[r12]);
  fault_handler_printf(msg);
  sprintf(msg, "lr  = 0x%08x\n", stack[lr]);
  fault_handler_printf(msg);
  sprintf(msg, "pc  = 0x%08x\n", stack[pc]);
  fault_handler_printf(msg);
  sprintf(msg, "psr = 0x%08x\n", stack[psr]);
  fault_handler_printf(msg);
}

void base_fault_handler(uint32_t stack[]) {
  // The implementation of these fault handler printf methods will depend on
  // how you have set your microcontroller up for debugging - they can either
  // be semihosting instructions, write data to ITM stimulus ports if you
  // are using a CPU that supports TRACESWO, or maybe write to a dedicated
  // debug UART
  fault_handler_printf("Fault encountered!\n");
  static char buf[64];
  // Get the fault cause. Volatile to prevent compiler elision.
  const volatile uint8_t active_interrupt = ICSR & 0xFF;
  // Interrupt numbers below 16 are core system interrupts, we know their names
  if (active_interrupt < 16) {
    sprintf(buf, "Cause: %s (%u)\n", system_interrupt_names[active_interrupt],
             active_interrupt);
  } else {
    // External (user) interrupt. Must be looked up in the datasheet specific
    // to this processor / microcontroller.
    sprintf(buf, "Unimplemented user interrupt %u\n", active_interrupt - 16);
  }
  fault_handler_printf(buf);

  fault_handler_printf("Saved register state:\n");
  dump_registers(stack);
  __asm volatile("BKPT #01");
  while (1) {
  }
}

void hard_fault_handler(void) {
  __asm(
    "MRS r0, MSP\n" // Default to the Main Stack Pointer
    "MOV r1, lr\n"  // Load the current link register value
    "MOVS r2, #4\n" // Load constant 4
    "TST r1, r2\n"  // Test whether we are in master or thread mode
    "BEQ base_fault_handler\n" // If in master mode, MSP is correct.
    "MRS r0, PSP\n" // If we weren't in master mode, load PSP instead
    "B base_fault_handler"); // Jump to the fault handler.
}

