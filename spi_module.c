#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"
#include "nordic_common.h"
#include "bsp.h"
#include "spi_module.h"

#define TX_RX_BUF_LENGTH         16u                 /**< SPI transaction buffer length. */

#if (SPI0_ENABLED == 1)
static const nrf_drv_spi_t m_spi_master = NRF_DRV_SPI_INSTANCE(0);
#elif (SPI1_ENABLED == 1)
static const nrf_drv_spi_t m_spi_master = NRF_DRV_SPI_INSTANCE(1);
#elif (SPI2_ENABLED == 1)
static const nrf_drv_spi_t m_spi_master = NRF_DRV_SPI_INSTANCE(2);
#else
#error "No SPI enabled."
#endif


// Data buffers.
static uint8_t m_tx_data[TX_RX_BUF_LENGTH] = {0}; /**< A buffer with data to transfer. */
static uint8_t m_rx_data[TX_RX_BUF_LENGTH] = {0}; /**< A buffer for incoming data. */

static volatile bool m_transfer_completed = true; /**< A flag to inform about completed transfer. */



/**@brief Function for SPI master event callback.
 *
 * Upon receiving an SPI transaction complete event, checks if received data are valid.
 *
 * @param[in] spi_master_evt    SPI master driver event.
 */
static void spi_master_event_handler(nrf_drv_spi_event_t event)
{
    switch (event) {
    case NRF_DRV_SPI_EVENT_DONE:
        // Inform application that transfer is completed.
        m_transfer_completed = true;
        break;

    default:
        // No implementation needed.
        break;
    }
}


void spi_init(uint32_t clk, uint32_t mosi)
{
    nrf_drv_spi_config_t const config = {
        .sck_pin  = clk,
        .mosi_pin = mosi,
        .miso_pin = NRF_DRV_SPI_PIN_NOT_USED, // SPI0_CONFIG_MISO_PIN,
        .ss_pin   = NRF_DRV_SPI_PIN_NOT_USED, // SPI0_CONFIG_CS_PIN,
        .irq_priority = APP_IRQ_PRIORITY_LOW,
        .orc          = 0xCC,
        .frequency    = NRF_DRV_SPI_FREQ_8M,
        .mode         = NRF_DRV_SPI_MODE_3,
        .bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST,
    };
    ret_code_t err_code = nrf_drv_spi_init(&m_spi_master, &config, spi_master_event_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Functions prepares buffers and starts data transfer.
 *
 * @param[in] p_tx_data     A pointer to a buffer TX.
 * @param[in] p_rx_data     A pointer to a buffer RX.
 * @param[in] len           A length of the data buffers.
 */
void spi_send_recv(uint8_t * const p_tx_data,
                   uint8_t * const p_rx_data,
                   const uint16_t len)
{
    m_transfer_completed = false;
    // Start transfer.
    nrf_drv_spi_transfer(&m_spi_master,
                         p_tx_data, len, p_rx_data, len);
    while (!m_transfer_completed);
}


uint8_t* spi_transfer(uint8_t * message, const uint16_t len)
{
    memcpy((void*)m_tx_data, (void*)message, len);
    spi_send_recv(m_tx_data, m_rx_data, len);
    return m_rx_data;
}

#if 0
void spi_send(uint8_t * message, const uint16_t len)
{
    memcpy((void*)m_tx_data, (void*)message, len);
    spi_send_recv(m_tx_data, m_rx_data, 0);
}
#endif
