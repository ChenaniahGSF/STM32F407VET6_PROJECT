/*
 * xmodem.c
 *
 *  Created on: Oct 26, 2025
 *      Author: Diamo
 */
#include "xmodem.h"
#include "usart.h"
#include "smarttimer.h"
#include "logger.h"

#ifdef X_ENABLE_DEBUG
#define xmodem_debug(...) logger_debug(__VA_ARGS__)
#define xmodem_hex(...) logger_hex(__VA_ARGS__)
#else
#define xmodem_debug(...)
#define xmodem_hex(...)
#endif

volatile bool xmodem_received = false;
volatile bool xmodem_send_echo_flag = true;
volatile uint16_t xmodem_received_size = 0;
uint8_t xmodem_echo_event = 0;
uint8_t* xmodem_receive_ptr = NULL;
#ifdef XMODEM_USE_USART1
#else
uint8_t xmodem_receive_buffer[X_PACKET_PROTOCOL_SIZE+X_PACKET_NUMBER_SIZE+X_PACKET_1024_SIZE+X_PACKET_CRC_SIZE];
#endif

#ifdef XMODEM_USE_USART1
static void xmodem_transmit_ch(uint8_t ch)
{
  HAL_UART_Transmit(&huart1, &ch, 1, 100);
}
#else
static void xmodem_transmit_ch(uint8_t ch)
{
  HAL_UART_Transmit(&huart2, &ch, 1, 100);
}
#endif


#if 0
//echo send X_NAK(0x15)
uint8_t XModem_Checksum(uint8_t *data, uint32_t len) {
    uint8_t checksum = 0;

    for (uint32_t i = 0; i < len; i++) {
        checksum += data[i];
    }

    return checksum;
}
#endif

static uint16_t xmodem_calc_crc16(uint8_t *data, uint16_t length)
{
    uint16_t crc = 0u;
    while (length) {
        length--;
        crc = crc ^ ((uint16_t)*data++ << 8u);
        for (uint8_t i = 0u; i < 8u; i++) {
            if (crc & 0x8000u) {
                crc = (crc << 1u) ^ 0x1021u;
            } else {
                crc = crc << 1u;
            }
        }
    }
    return crc;
}

static void xmodem_send_echo_callback(void)
{
  //xmodem_transmit_ch(X_NAK);
  xmodem_transmit_ch(X_C);
}

void xmodem_start_send_echo(void)
{
  xmodem_send_echo_flag = true;
  xmodem_echo_event = stim_loop(3000, xmodem_send_echo_callback, STIM_LOOP_FOREVER);
  xmodem_debug("xmodem_echo_event = %d", xmodem_echo_event);
}

void xmodem_stop_send_echo(void)
{
  xmodem_send_echo_flag = false;
  stim_kill_event(xmodem_echo_event);  //directly remove the event
  xmodem_debug("xmodem kill event = %d", xmodem_echo_event);
}

#ifdef XMODEM_USE_USART1
void xmodem_receive_callback(uint8_t* in, uint16_t size)
{
  if(xmodem_received == false) {
  if(xmodem_send_echo_flag == true) {
    xmodem_stop_send_echo();
    xmodem_send_echo_flag = false;
  }
  xmodem_receive_ptr = in;
  xmodem_received_size = size;
  xmodem_debug("xmodem received size = %d", xmodem_received_size);

  xmodem_received = true;
  }
}
#else
void xmodem_receive_callback(uint16_t size)
{
  if(xmodem_received == false) {
    if(xmodem_send_echo_flag == true) {
      xmodem_stop_send_echo();
      xmodem_send_echo_flag = false;
    }
    xmodem_received_size = size;
    xmodem_debug("xmodem received size = %d", xmodem_received_size);

    HAL_UARTEx_ReceiveToIdle_IT(&huart2, xmodem_receive_buffer, sizeof(xmodem_receive_buffer));
    xmodem_received = true;
  }
}

void xmodem_init(void)
{
  xmodem_receive_ptr = xmodem_receive_buffer;

  //will enable TC IDLE event
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, xmodem_receive_buffer, sizeof(xmodem_receive_buffer));
  xmodem_start_send_echo();
}
#endif

static void xmodem_error_handler(uint8_t *error_number, uint8_t max_error_number)
{
  /* Raise the error counter. */
  (*error_number)++;
  /* If the counter reached the max value, then abort. */
  if ((*error_number) >= max_error_number)
  {
    /* Graceful abort. */
    xmodem_transmit_ch(X_CAN);
    xmodem_transmit_ch(X_CAN);
    (*error_number) = 0;
  }
  /* Otherwise send a NAK for a repeat. */
  else
  {
    xmodem_transmit_ch(X_NAK);
  }
  xmodem_stop_send_echo();
  xmodem_start_send_echo();
}

void xmodem_receive_mainfuncion(void) {
  static volatile xmodem_status status = X_OK;
  static uint8_t error_number = 0u;
  uint16_t package_size = 0;

  if(xmodem_received == true) {
    uint8_t header = xmodem_receive_ptr[0];
    switch(header) {
      case X_SOH:
      case X_STX:
        status = X_OK;
        if(header == X_SOH) {
          package_size = X_PACKET_128_SIZE;
        } else if(header == X_STX) {
          package_size = X_PACKET_1024_SIZE;
        } else {
          status |= X_ERROR;
        }
        if((xmodem_receive_ptr[X_PACKET_PROTOCOL_SIZE] == 0x00) && (xmodem_receive_ptr[X_PACKET_PROTOCOL_SIZE+1] == 0xFF) && (xmodem_receive_ptr[X_PACKET_PROTOCOL_SIZE+X_PACKET_NUMBER_SIZE] != 0x00)) {
          xmodem_debug("ymodem first frame");
          xmodem_hex(xmodem_receive_buffer, xmodem_received_size);
          xmodem_start_send_echo();
        }
        if((xmodem_receive_ptr[X_PACKET_PROTOCOL_SIZE] == 0x00) && (xmodem_receive_ptr[X_PACKET_PROTOCOL_SIZE+1] == 0xFF) && (xmodem_receive_ptr[X_PACKET_PROTOCOL_SIZE+X_PACKET_NUMBER_SIZE] == 0x00)) {
          xmodem_debug("ymodem end zero frame");
          xmodem_hex(xmodem_receive_buffer, xmodem_received_size);
          xmodem_start_send_echo();
        }
        if((xmodem_received_size-package_size) != (X_PACKET_PROTOCOL_SIZE+X_PACKET_NUMBER_SIZE+X_PACKET_CRC_SIZE)) {
          status |= X_ERROR_NUMBER;
        }
        if((xmodem_receive_ptr[X_PACKET_NUMBER_INDEX] + xmodem_receive_ptr[X_PACKET_NUMBER_COMPLEMENT_INDEX]) != 255) {
          status |= X_ERROR_INDEX;
        }
        uint16_t crc_received = (xmodem_receive_ptr[X_PACKET_PROTOCOL_SIZE+X_PACKET_NUMBER_SIZE+package_size] << 8u) + xmodem_receive_ptr[X_PACKET_PROTOCOL_SIZE+X_PACKET_NUMBER_SIZE+package_size+1];
        uint16_t crc_calucated = xmodem_calc_crc16(&xmodem_receive_ptr[X_PACKET_PROTOCOL_SIZE+X_PACKET_NUMBER_SIZE],  package_size);
        xmodem_debug("crc_received = %d, crc_calucated = %d", crc_received, crc_calucated);
        if(crc_received != crc_calucated) {
          status |= X_ERROR_CRC;
        }
        if(status == X_OK) {
          xmodem_transmit_ch(X_ACK);
        } else {
          xmodem_error_handler(&error_number, X_MAX_ERRORS);
          HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
        }
        break;
      case X_EOT:
        status = X_OK;
        xmodem_transmit_ch(X_ACK);
        xmodem_debug("xmodem received success");
        xmodem_start_send_echo();
        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
        break;
      case X_CAN:
        status = X_ERROR;
        xmodem_start_send_echo();
        break;
      default:
        status = X_ERROR;
        xmodem_error_handler(&error_number, X_MAX_ERRORS);
        break;
    }

    xmodem_received = false;
  }
}
