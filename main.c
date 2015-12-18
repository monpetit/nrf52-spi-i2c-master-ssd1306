/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**@file
 * @defgroup spi_master_example_with_slave_main main.c
 * @{
 * @ingroup spi_master_example
 *
 * @brief SPI master example application to be used with the SPI slave example application.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "app_error.h"
#include "app_util_platform.h"
#include "nrf_delay.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_drv_spi.h"
#include "nordic_common.h"
#include "uart_module.h"
#include "spi_module.h"
#include "ssd1306.h"
#include "binary.h"


#define DELAY_MS                 1000                /**< Timer Delay in milli-seconds. */

/*
 * This example uses only one instance of the SPI master.
 * Please make sure that only one instance of the SPI master is enabled in config file.
 */

#define APP_TIMER_PRESCALER      0                      /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS     BSP_APP_TIMERS_NUMBER  /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE  2                      /**< Size of timer operation queues. */


volatile bool change_rtc = false;
extern uint8_t time_buffer[128];

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

static const unsigned char /*PROGMEM*/ logo16_glcd_bmp[] = {
    B00000000, B11000000,
    B00000001, B11000000,
    B00000001, B11000000,
    B00000011, B11100000,
    B11110011, B11100000,
    B11111110, B11111000,
    B01111110, B11111111,
    B00110011, B10011111,
    B00011111, B11111100,
    B00001101, B01110000,
    B00011011, B10100000,
    B00111111, B11100000,
    B00111111, B11110000,
    B01111100, B11110000,
    B01110000, B01110000,
    B00000000, B00110000
};


#define SSD1306_CONFIG_VDD_PIN      2
#define SSD1306_CONFIG_CLK_PIN      23
#define SSD1306_CONFIG_MOSI_PIN     24
#define SSD1306_CONFIG_CS_PIN       22
#define SSD1306_CONFIG_DC_PIN       20
#define SSD1306_CONFIG_RST_PIN      19

#define SSD1306_CONFIG_SCL_PIN      27
#define SSD1306_CONFIG_SDA_PIN      26

/**@brief Function for error handling, which is called when an error has occurred.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name.
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    UNUSED_VARIABLE(bsp_indication_set(BSP_INDICATE_FATAL_ERROR));

    for (;;) {
        // No implementation needed.
    }
}



/**@brief Function for initializing bsp module.
 */
void bsp_configuration()
{
    uint32_t err_code = NRF_SUCCESS;

    NRF_CLOCK->LFCLKSRC            = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
        // Do nothing.
    }

    APP_TIMER_INIT(APP_TIMER_PRESCALER, /*APP_TIMER_MAX_TIMERS,*/ APP_TIMER_OP_QUEUE_SIZE, NULL);

    err_code = bsp_init(BSP_INIT_LED, APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
    APP_ERROR_CHECK(err_code);
}


void testdrawchar(void)
{
    ssd1306_clear_display();
    ssd1306_set_textsize(1);
    ssd1306_set_textcolor(WHITE);
    ssd1306_set_cursor(0, 0);

    for (uint8_t i = 0; i < 168; i++) {
        if (i == '\n') continue;
        ssd1306_write(i);
        if ((i > 0) && (i % 21 == 0))
            ssd1306_write('\n');
    }
    ssd1306_display();
}


void testdrawline(void)
{
    for (int16_t i = 0; i < ssd1306_width(); i += 4) {
        ssd1306_draw_line(0, 0, i, ssd1306_height() - 1, WHITE);
        ssd1306_display();
    }
    for (int16_t i = 0; i < ssd1306_height(); i += 4) {
        ssd1306_draw_line(0, 0, ssd1306_width() - 1, i, WHITE);
        ssd1306_display();
    }
    nrf_delay_ms(250);

    ssd1306_clear_display();
    for (int16_t i = 0; i < ssd1306_width(); i += 4) {
        ssd1306_draw_line(0, ssd1306_height() - 1, i, 0, WHITE);
        ssd1306_display();
    }
    for (int16_t i = ssd1306_height() - 1; i >= 0; i -= 4) {
        ssd1306_draw_line(0, ssd1306_height() - 1, ssd1306_width() - 1, i, WHITE);
        ssd1306_display();
    }
    nrf_delay_ms(250);

    ssd1306_clear_display();
    for (int16_t i = ssd1306_width() - 1; i >= 0; i -= 4) {
        ssd1306_draw_line(ssd1306_width() - 1, ssd1306_height() - 1, i, 0, WHITE);
        ssd1306_display();
    }
    for (int16_t i = ssd1306_height() - 1; i >= 0; i -= 4) {
        ssd1306_draw_line(ssd1306_width() - 1, ssd1306_height() - 1, 0, i, WHITE);
        ssd1306_display();
    }
    nrf_delay_ms(250);

    ssd1306_clear_display();
    for (int16_t i = 0; i < ssd1306_height(); i += 4) {
        ssd1306_draw_line(ssd1306_width() - 1, 0, 0, i, WHITE);
        ssd1306_display();
    }
    for (int16_t i = 0; i < ssd1306_width(); i += 4) {
        ssd1306_draw_line(ssd1306_width() - 1, 0, i, ssd1306_height() - 1, WHITE);
        ssd1306_display();
    }
    nrf_delay_ms(250);

    ssd1306_display();
    nrf_delay_ms(250);
    ssd1306_clear_display();
}


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h)
{
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

    uint8_t icons[NUMFLAKES][3];

    // initialize
    for (uint8_t f = 0; f < NUMFLAKES; f++) {
        icons[f][XPOS] = rand() % ssd1306_width();
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = (rand() % 5) + 1;
    }

    while (1) {
        // draw each icon
        for (uint8_t f = 0; f < NUMFLAKES; f++) {
            ssd1306_draw_bitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, WHITE);
        }
        ssd1306_display();
        nrf_delay_ms(200);

        // then erase it + move it
        for (uint8_t f = 0; f < NUMFLAKES; f++) {
            ssd1306_draw_bitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, BLACK);
            // move it
            icons[f][YPOS] += icons[f][DELTAY];
            // if its gone, reinit
            if (icons[f][YPOS] > ssd1306_height()) {
                icons[f][XPOS] = rand() % ssd1306_width();
                icons[f][YPOS] = 0;
                icons[f][DELTAY] = (rand() % 5) + 1;
            }
        }
    }
}

void ssd1306_power_on(void)
{
    nrf_gpio_pin_set(SSD1306_CONFIG_VDD_PIN); // vdd
    nrf_gpio_cfg(
        SSD1306_CONFIG_VDD_PIN,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_H0H1, // NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
}


/**@brief Function for application main entry. Does not return. */
int main(void)
{
    // Setup bsp module.
    bsp_configuration();

    uart_init();
#if 0
    ssd1306_power_on();
    ssd1306_init(SSD1306_CONFIG_DC_PIN,
                 SSD1306_CONFIG_RST_PIN,
                 SSD1306_CONFIG_CS_PIN,
                 SSD1306_CONFIG_CLK_PIN,
                 SSD1306_CONFIG_MOSI_PIN);

    ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS, true);
#else
    ssd1306_init_i2c(SSD1306_CONFIG_SCL_PIN, SSD1306_CONFIG_SDA_PIN);
    ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS, false);
#endif

    nrf_delay_ms(1000);
    puts("--- START ---");

    nrf_delay_ms(1000);
    ssd1306_display();
    nrf_delay_ms(DELAY_MS);

    testdrawline();

    for (;;) {
        ssd1306_clear_display();
        // draw a single pixel
        ssd1306_draw_pixel(10, 10, WHITE);
        ssd1306_display();
        nrf_delay_ms(DELAY_MS);

        ssd1306_draw_circle(SSD1306_LCDWIDTH / 2, SSD1306_LCDHEIGHT / 2, 30, WHITE);
        ssd1306_display();
        nrf_delay_ms(DELAY_MS);

        testdrawchar();
        nrf_delay_ms(DELAY_MS);

        ssd1306_clear_display();
        ssd1306_display();
        nrf_delay_ms(DELAY_MS);

        testdrawline();

        ssd1306_clear_display();
        ssd1306_draw_bitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
        ssd1306_display();
        nrf_delay_ms(DELAY_MS);

        // draw a bitmap icon and 'animate' movement
        testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
    }
}


/** @} */
