/*
 * xmodem.h
 *
 *  Created on: Oct 26, 2025
 *      Author: Diamo
 */

#ifndef XMODEM_INC_XMODEM_H_
#define XMODEM_INC_XMODEM_H_
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define XMODEM_USE_USART1

#define X_MAX_ERRORS ((uint8_t)3u)

#define X_PACKET_PROTOCOL_SIZE  ((uint16_t)1u)
#define X_PACKET_NUMBER_SIZE    ((uint16_t)2u)
#define X_PACKET_128_SIZE       ((uint16_t)128u)
#define X_PACKET_1024_SIZE      ((uint16_t)1024u)
#define X_PACKET_CRC_SIZE       ((uint16_t)2u)

/* Indexes inside packets. */
#define X_PACKET_NUMBER_INDEX             ((uint16_t)1u)
#define X_PACKET_NUMBER_COMPLEMENT_INDEX  ((uint16_t)2u)

/* Bytes defined by the protocol. */
#define X_SOH ((uint8_t)0x01u)  /**< Start Of Header (128 bytes). */
#define X_STX ((uint8_t)0x02u)  /**< Start Of Header (1024 bytes). */
#define X_EOT ((uint8_t)0x04u)  /**< End Of Transmission. */
#define X_ACK ((uint8_t)0x06u)  /**< Acknowledge. */
#define X_NAK ((uint8_t)0x15u)  /**< Not Acknowledge. */
#define X_CAN ((uint8_t)0x18u)  /**< Cancel. */
#define X_C   ((uint8_t)0x43u)  /**< ASCII "C" to notify the host we want to use CRC16. */

//#define X_ENABLE_DEBUG

/* Status report for the functions. */
typedef enum {
  X_OK            = 0x00u, /**< The action was successful. */
  X_ERROR_CRC     = 0x01u, /**< CRC calculation error. */
  X_ERROR_NUMBER  = 0x02u, /**< Packet number mismatch error. */
  X_ERROR_INDEX   = 0x04u, /**< UART communication error. */
  X_ERROR         = 0xFFu  /**< Generic error. */
} xmodem_status;

void xmodem_start_send_echo(void);
void xmodem_stop_send_echo(void);

#ifdef XMODEM_USE_USART1
void xmodem_receive_callback(uint8_t* in, uint16_t size);
#else
void xmodem_receive_callback(uint16_t size);
void xmodem_init(void);
#endif
void xmodem_receive_mainfuncion(void);

#endif /* XMODEM_INC_XMODEM_H_ */
