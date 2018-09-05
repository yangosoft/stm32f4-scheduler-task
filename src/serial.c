#include <stdint.h>

#define UART0_R ((uint32_t *) 0x4000C000)

#define UART0_DR_R              (*((volatile unsigned long *)0x4000C000))
#define UART0_FR_R              (*((volatile unsigned long *)0x4000C018))
#define UART0_IBRD_R            (*((volatile unsigned long *)0x4000C024))
#define UART0_FBRD_R            (*((volatile unsigned long *)0x4000C028))
#define UART0_LCRH_R            (*((volatile unsigned long *)0x4000C02C))
#define UART0_CTL_R             (*((volatile unsigned long *)0x4000C030))

#define SYSCTL_RCGC1_R          (*((volatile unsigned long *)0x400FE104))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

#define GPIO_PORTA_DATA_R       (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R        (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R      (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_DEN_R        (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_AMSEL_R      (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile unsigned long *)0x4000452C))
#define GPIO_PORTA_DR8R_R       (*((volatile unsigned long *)0x40004508))

void init_uart0() {
    // Step 1:
    //   SYSCTL_RCGC1_R |= 0x00000001; // activate UART0
    //   SYSCTL_RCGC2_R |= 0x00000001; // activate port A

    // Step 2:
    //   UART0_CTL_R &= ~0x00000001; // disable UART
    //   UART0_IBRD_R = 27;  // IBRD = int(50,000,000 / (16 * 115,200)) = int(27.1267)
    //   UART0_FBRD_R = 8;   // FBRD = int(0.1267 * 64 + 0.5) = 8                     
    //   UART0_LCRH_R = (0x00000060|0x00000010); // 8 bit word length 
    //   UART0_CTL_R |= 0x00000001; // enable UART

    // Step 3:
    GPIO_PORTA_AFSEL_R |= 0x03; // enable alt funct on PA1-0
    GPIO_PORTA_DEN_R |= 0x03; // enable digital I/O on PA1-0                                  
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) + 0x00000011; // configure PA1-0 as UART
    GPIO_PORTA_AMSEL_R &= ~0x03; // disable analog functionality on PA
}

int putchar(int c) {
    while ((UART0_FR_R & 0x00000020) != 0); // Step 1    
    *UART0_R = c;
}

int puts(unsigned char *str) {
    int i = 0;

    while (str[i]) {
        console_putc(str[i]);

        i++;
    }
}
