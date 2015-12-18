#ifndef __SPI_MODULE_H
#define __SPI_MODULE_H

#include <stdint.h>

#ifdef __splusplus
extern "C" {
#endif

    void spi_init(uint32_t clk, uint32_t mosi);

    void spi_send_recv(uint8_t * const p_tx_data,
                       uint8_t * const p_rx_data,
                       const uint16_t len);

    uint8_t* spi_transfer(uint8_t * message, const uint16_t len);
#if 0
    void spi_send(uint8_t * message, const uint16_t len);
#endif

#ifdef __splusplus
}
#endif

#endif /* __SPI_MODULE_H */

