/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>,
 * Copyright (C) 2010-2015 Piotr Esden-Tempski <piotr@esden.net>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
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





#include <string>

extern "C"
{
#include <stdio.h>
#include <ctype.h>
    
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/systick.h>
    
    
#include "console.h"
#include "clock.h"
#include "sdram.h"
#include "lcd.h"
#include "gfx.h"
}

/* Set STM32 to 168 MHz. */
//static void clock_setup(void)
//{
//	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
//}


static void adc_setup(void)
{
        rcc_periph_clock_enable(RCC_ADC1);
// 	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);
        gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO2);

	adc_power_off(ADC1);
	adc_disable_scan_mode(ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_3CYC);

	adc_power_on(ADC1);

}

static uint16_t read_adc_naiive(uint8_t channel)
{
	uint8_t channel_array[16];
	channel_array[0] = channel;
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_start_conversion_regular(ADC1);
	while (!adc_eoc(ADC1));
	uint16_t reg16 = adc_read_regular(ADC1);
	return reg16;
}

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

int Amain(void)
{
    using namespace std;
	int i;

	clock_setup();
	button_setup();
	gpio_setup();
        adc_setup();

        /* Enable the SDRAM attached to the board */
	sdram_init();
	/* Enable the LCD attached to the board */
        lcd_init();

	console_setup(115200);
	printf("System initialized.\n");
        
        
        gfx_init(lcd_draw_pixel, 240, 320);
	gfx_fillScreen(LCD_GREY);
	gfx_fillRoundRect(10, 10, 220, 220, 5, LCD_WHITE);
	gfx_drawRoundRect(10, 10, 220, 220, 5, LCD_RED);
	gfx_fillCircle(20, 250, 10, LCD_RED);
	gfx_fillCircle(120, 250, 10, LCD_GREEN);
	gfx_fillCircle(220, 250, 10, LCD_BLUE);
	gfx_setTextSize(2);
	gfx_setCursor(15, 25);
	gfx_puts(R"(STM32F4-DISCO)");
	gfx_setTextSize(1);
	gfx_setCursor(15, 49);
	gfx_puts(R"(Simple example to put some)");
	gfx_setCursor(15, 60);
	gfx_puts(R"(stuff on the LCD screen.)");
	lcd_show_frame();
        
        char buffer[256];
        
	/* Blink the LED (PD13) on the board. */
	while (1) {
		gpio_toggle(GPIOG, GPIO13);
                gpio_toggle(GPIOG, GPIO14);
                
            
                
                uint16_t input_adc0 = read_adc_naiive(1);
		
                
                
                gfx_fillScreen(LCD_GREY);
                gfx_setTextSize(3);
                gfx_setCursor(15, 80);
                
                sprintf(buffer, "ADC: %d", input_adc0);
                gfx_puts( buffer );
                
                lcd_show_frame();
        
                gpio_toggle(GPIOG, GPIO14);
                

		/* Upon button press, blink more slowly. */
// 		if (gpio_get(GPIOA, GPIO0)) {
//                     msleep(1000);
// 		}
// 
//                 
//                 msleep(500);

	}

	return 0;
}
