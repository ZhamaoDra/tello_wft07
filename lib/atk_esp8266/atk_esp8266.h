#ifndef _ATK_ESP8266_H
#define _ATK_ESP8266_H

#include <stm32f1xx_hal.h>

#define ATK_ESP8266_MAX_RECV_LEN 400
#define ATK_ESP8266_MAX_TRAN_LEN 400

char atk_esp8266_rx_buff[ATK_ESP8266_MAX_RECV_LEN];
char atk_esp8266_tx_buff[ATK_ESP8266_MAX_TRAN_LEN];

extern UART_HandleTypeDef huart2;
#define ATK_ESP8266_UART huart2

HAL_StatusTypeDef SendCmd_ATK_ESP8266(const char *cmd, const char *ack, uint16_t timeout);
HAL_StatusTypeDef printf_ATK_ESP8266(const char *fmt, ...);
HAL_StatusTypeDef SendData_ATK_ESP8266(uint8_t *data, uint16_t size, uint32_t timeout);
HAL_StatusTypeDef Init_ATK_ESP8266();
#endif