#include "nrf.h"
#include "app_uart.h"
// #include "boards.h"
#include "app_error.h"
#include "bsp.h"
#include "nordic_common.h"
#include "uart_module.h"
#include "ds3234_rtc.h"

#include <stdio.h>
#include <stdbool.h>

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */

uint8_t time_buffer[128] = {0};
extern volatile bool change_rtc;



/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' i.e '\n' (hex 0x0D) or if the string has reached a length of
 *          @ref NUS_MAX_DATA_LENGTH.
 */
/**@snippet [Handling the data received over UART] */
void uart_event_handle(app_uart_evt_t * p_event)
{
    static uint8_t index = 0;
    static uint8_t ch;

    switch (p_event->evt_type) {
    case APP_UART_DATA_READY:
        UNUSED_VARIABLE(app_uart_get(&ch));
        app_uart_put(ch);
        if (ch == '{') {
            index = 0;
            time_buffer[index++] = ch;
        }
        else if (ch == '}') {
            time_buffer[index] = '\0';
            index = 0;
            printf("\r\n* received string = [%s]\r\n", time_buffer + 1);
            change_rtc = true;
        }
        else if (index >= 128)
            index = 0;
        else
            time_buffer[index++] = ch;
        break;

    case APP_UART_COMMUNICATION_ERROR:
        APP_ERROR_HANDLER(p_event->data.error_communication);
        break;

    case APP_UART_FIFO_ERROR:
        APP_ERROR_HANDLER(p_event->data.error_code);
        break;

    default:
        break;
    }
}
/**@snippet [Handling the data received over UART] */



/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
void uart_init(void)
{
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params = {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_ENABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud38400
    };

    APP_UART_FIFO_INIT( &comm_params,
                        UART_RX_BUF_SIZE,
                        UART_TX_BUF_SIZE,
                        uart_event_handle,
                        APP_IRQ_PRIORITY_LOW,
                        err_code);
    APP_ERROR_CHECK(err_code);
}
/**@snippet [UART Initialization] */

