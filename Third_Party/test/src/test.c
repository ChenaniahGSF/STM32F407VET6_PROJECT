/*
 * test.c
 *
 *  Created on: Oct 19, 2025
 *      Author: Diamo
 */
#include "ee24.h"
#include "spif.h"
#include "logger.h"
#include "test.h"
#include "ee.h"

EE24_HandleTypeDef ee24;
uint8_t erase_data[4] = {0xff, 0xff, 0xff, 0xff};
uint8_t in_data1[4] = {0x3, 0x3, 0x3, 0x3};
uint8_t in_data2[4] = {0xc, 0xc, 0xc, 0xc};
uint8_t out_data[4];

void at24cxx_test(void) {
  if(EE24_Init(&ee24, &hi2c1, EE24_ADDRESS_DEFAULT)){
    EE24_Write(&ee24, 0x0, erase_data, sizeof(erase_data), 1000);
    HAL_Delay(10);

    EE24_Write(&ee24, 0x0, in_data1, sizeof(in_data1), 1000);
    HAL_Delay(10);

    EE24_Read(&ee24, 0x0, out_data, sizeof(out_data), 1000);
    HAL_Delay(10);
    logger_hex(out_data, sizeof(out_data));

    EE24_Write(&ee24, 0x0, in_data2, sizeof(in_data2), 1000);
    HAL_Delay(10);

    EE24_Read(&ee24, 0x0, out_data, sizeof(out_data), 1000);
    HAL_Delay(10);
    logger_hex(out_data, sizeof(out_data));

  } else {
    logger_error("EE24_Init failed..");
  }
};

uint8_t spi_read[256];
uint8_t spi_write[256];
SPIF_HandleTypeDef spif;

void w25qxx_test(void) {
  if(SPIF_Init(&spif, &hspi3, SPI3_CS_GPIO_Port, SPI3_CS_Pin)) {
    for(int i=0; i<256; i++) {
      spi_write[i] = i;
    }
    logger_info("spi_write:");
    logger_hex(spi_write, sizeof(spi_write));

    SPIF_EraseSector(&spif, 0);
    SPIF_WritePage(&spif, 0, spi_write, sizeof(spi_write), 0);
    SPIF_ReadPage(&spif, 0, spi_read, sizeof(spi_read), 0);
    logger_info("spi_read:");
    logger_hex(spi_read, sizeof(spi_read));

  } else {
    logger_error("SPIF_Init failed..");
  }
}

typedef struct
{
 uint32_t val1;
 int16_t val2;
 int8_t val3;
 float val4;
} stotrage_t;

void ee_test(void) {

  stotrage_t ee_data;
  ee_init(&ee_data, sizeof(stotrage_t));
  ee_read();
  logger_info("val1: %u, val2: %d, val3: %d, val4: %f", ee_data.val1, ee_data.val2, ee_data.val3, ee_data.val4);
#if 0
  ee_data.val1 = 10000;
  ee_data.val2 = -202;
  ee_data.val3 = -3;
  ee_data.val4 = 4.5f;
  ee_write();
  HAL_Delay(100);
  ee_read();
  logger_info("val1: %u, val2: %d, val3: %d, val4: %f", ee_data.val1, ee_data.val2, ee_data.val3, ee_data.val4);
#endif
}

