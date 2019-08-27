

#include <task.h>
#include <stdio.h>
#include <ctype.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include <libopencm3/cm3/nvic.h>

#include "console.h"
#include "sdram.h"
#include "lcd.h"
#include "gfx.h"

void clock_setup(void) {
    rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
    // 
    // 	/* set up the SysTick function (1mS interrupts) */
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    STK_CVR = 0;
    systick_set_reload(rcc_ahb_frequency / 1000);
    systick_counter_enable();
    //  	systick_interrupt_enable();
}

static void gpio_setup(void) {
    /* Enable GPIOG clock. */
    rcc_periph_clock_enable(RCC_GPIOG);

    /* Set GPIO13 (in GPIO port G) to 'output push-pull'. */
    gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT,
            GPIO_PUPD_NONE, GPIO13);
    gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT,
            GPIO_PUPD_NONE, GPIO14);
}

/* simple millisecond counter */
static volatile uint32_t system_millis;
static volatile uint32_t delay_timer;

/*
 * Simple systick handler
 *
 * Increments a 32 bit value once per millesecond
 * which rolls over every 49 days.
 */
void sys_tick_handler(void) {
    system_millis++;
    puts("OK");
    if (delay_timer > 0) {
        delay_timer--;
    }
}


#define TIMER_RELOAD2 0x00fffff
#define TIMER_RELOAD  0x3ffffff

void task_1(void) {
    while (1) {
        puts("TASK_1\n");
        gpio_toggle(GPIOG, GPIO13);
    }
}

void task_2(void) {
    while (1) {
        puts("TASK_2\n");
        gpio_toggle(GPIOG, GPIO14);
    }
}

void task_3(void) {
    while (1) {
        puts("TASK_3\n");
        gfx_fillScreen(LCD_YELLOW);
    }
}

int main() {



    task_init();
    int i = task_create(task_1);
    if (i == -1) {
        puts("Error creating task\n");
    }
    i = task_create(task_2);
    if (i == -1) {
        puts("Error creating task\n");
    }
    i = task_create(task_3);
    if (i == -1) {
        puts("Error creating task\n");
    }
    clock_setup();
    gpio_setup();
    console_setup(115200);

    /* Enable the SDRAM attached to the board */
    sdram_init();
    /* Enable the LCD attached to the board */
    lcd_init();
    gfx_init(lcd_draw_pixel, 240, 320);



    gfx_fillScreen(LCD_YELLOW);
    gfx_fillRoundRect(10, 10, 220, 220, 5, LCD_WHITE);
    lcd_show_frame();

    timer_start();


    while (1) {
        puts("Running main loop\n");
    }

    return 0;
}
