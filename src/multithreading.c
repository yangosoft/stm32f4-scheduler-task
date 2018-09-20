/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2015 Jonas Norling <jonas.norling@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencmsis/core_cm3.h>

#include "scheduler.h"
#include "console.h"
#include "clock.h"
#include "sdram.h"
#include "lcd.h"
#include "gfx.h"


/* Stack space for the tasks. Stacks on ARM should be 8-byte aligned. */
static uint8_t __attribute__((aligned(8))) stack1[2048];
static uint8_t __attribute__((aligned(8))) stack2[2048];



static void green_led_task(void);
static void blue_led_task(void);

const struct task_data tasks[SCHEDULER_NUM_TASKS] = {
		{ green_led_task, stack1, sizeof(stack1) },
		{ blue_led_task, stack2, sizeof(stack2) },
};

/* Current uptime in milliseconds */
volatile uint32_t uptime;


static float quicksine(float x)
{
	if (x < 0) {
		return 1.27323954f * x + .405284735f * x * x;
	} else {
		return 1.27323954f * x - 0.405284735f * x * x;
	}
}

/* Fade an LED in and out in a sinusoidal fashion. */
static void blinkloop(float speed, uint16_t ledpin)
{
    
	while (true) {
		float step;
		unsigned i;
                uint32_t j = 0;
		for (step = -3.14; step < 3.14; step += speed) {
			unsigned duty = 128 + 128 * quicksine(step);

			gpio_clear(GPIOG, ledpin); /* LED off */
			for (i = 0; i < 256; i++) {
				if (i == duty) {
					gpio_set(GPIOG, ledpin); /* LED on */
				}
 				j++;
                                printf("Task done %d\n",j);
				/* Let the other tasks run */
				scheduler_yield();
			}
		}
	}
}

static void f1(uint32_t gpio)
{
    int j = 0;
    
    while(true)
    {
        gfx_fillScreen(LCD_YELLOW|j);
	gfx_fillRoundRect(10, 10, 220, 220, 5, LCD_WHITE|gpio|j);
	gfx_drawRoundRect(10, 10, 220, 220, 5, LCD_RED|gpio|j);
	gfx_fillCircle(20, 250, j%10, LCD_RED*gpio|j);
	gfx_fillCircle(120, 250, j%10, LCD_GREEN|gpio|j);
	gfx_fillCircle(220, 250, j%10, LCD_BLUE|j);
	gfx_setTextSize(2);
	gfx_setCursor(15, 25);
	gfx_puts("(STM32F4-DISCO)");
	gfx_setTextSize(1);
	gfx_setCursor(15, 49);
	gfx_puts("(Simple example to put some)");
	gfx_setCursor(15, 60);
	gfx_puts("(stuff on the LCD screen.)");
	lcd_show_frame();
        
        //printf("%d\n",j);
        gpio_toggle(GPIOG, gpio);
        j++;
        scheduler_yield();
    }
}

static void green_led_task(void)
{
       
    f1(GPIO14);
//         printf("%s\n",__FUNCT
// 	blinkloop(0.005, GPIO13);
}

static void blue_led_task(void)
{
    f1(GPIO13);
}

/* ISR for the systick interrupt. Triggered every 1ms. We could invoke the
 * scheduler here to implement preemptive multithreading. */
void sys_tick_handler(void)
{
        Asys_tick_handler();
	uptime++;
}

// static void config_clocks(void)
// {
// 	rcc_clock_setup_in_hse_8mhz_out_24mhz();
// 
// 	/* Set systick timer to strike every 1ms (1kHz),
// 	 * enable its interrupt and start it. */
// 	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
// 	systick_set_reload(rcc_apb2_frequency / 8 / 1000 - 1);
// 	systick_interrupt_enable();
// 	systick_counter_enable();
// }

// static void config_gpio(void)
// {
// 	/* Enable GPIOC8 and GPIOC9 which are connected to the LEDs on the
// 	 * Discovery board */
// 	rcc_periph_clock_enable(RCC_GPIOC);
// 	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
// 			GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);
// 	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
// 			GPIO_CNF_OUTPUT_PUSHPULL, GPIO9);
// 
// 	gpio_set(GPIOC, GPIO8); /* Turn on blue */
// 	gpio_set(GPIOC, GPIO9); /* Turn on green */
// }


static void gpio_setup(void)
{
	/* Enable GPIOG clock. */
        rcc_periph_clock_enable(RCC_GPIOA);
        rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOG);

	/* Set GPIO13 (in GPIO port G) to 'output push-pull'. */
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT,GPIO_PUPD_NONE, GPIO13);
	gpio_mode_setup(GPIOG, GPIO_MODE_OUTPUT,GPIO_PUPD_NONE, GPIO14);
        
}

static void button_setup(void)
{
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Set GPIO0 (in GPIO port A) to 'input open-drain'. */
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO0);
}

int main(void)
{
	clock_setup();
	button_setup();
	gpio_setup();
        console_setup(115200);
         /* Enable the SDRAM attached to the board */
 	sdram_init();
	/* Enable the LCD attached to the board */
        lcd_init();
        
         gfx_init(lcd_draw_pixel, 240, 320);
	
        
	scheduler_init();
        printf("Init ok\n");
	/* Start running the tasks. This call will never return. */
	scheduler_yield();

	return 0;
}
