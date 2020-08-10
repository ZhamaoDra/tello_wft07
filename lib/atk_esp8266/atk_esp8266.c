#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "atk_esp8266.h"

// const uint8_t *wifi_ssid = "402";
// const uint8_t *wifi_password = "wu123456789";
// const uint8_t *udp_remote_ip = "192.168.0.4";

const uint8_t *wifi_ssid = "TELLO_zhamao";
const uint8_t *wifi_password = "wu123456789";
const uint8_t *udp_remote_ip = "192.168.10.1";
const uint16_t udp_remote_port = 8889;
const uint16_t udp_local_port = 8890;

HAL_StatusTypeDef printf_ATK_ESP8266(const char *fmt, ...)
{
    uint16_t len;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)atk_esp8266_tx_buff, fmt, ap);
    len = strlen((const char *)atk_esp8266_tx_buff);
    return HAL_UART_Transmit(&ATK_ESP8266_UART, (uint8_t *)atk_esp8266_tx_buff, len, 100);
}

HAL_StatusTypeDef SendCmd_ATK_ESP8266(const char *cmd, const char *ack, uint16_t timeout)
{
    printf_ATK_ESP8266("%s\r\n", cmd);
    if (ack && timeout)
        HAL_UART_Receive(&ATK_ESP8266_UART, (uint8_t *)atk_esp8266_rx_buff, ATK_ESP8266_MAX_RECV_LEN, timeout);
    if (strstr(atk_esp8266_rx_buff, ack))
        return HAL_OK;
    else
        return HAL_ERROR;
}

HAL_StatusTypeDef SendData_ATK_ESP8266(uint8_t *data, uint16_t size, uint32_t timeout)
{
    return HAL_UART_Transmit(&ATK_ESP8266_UART, data, size, timeout);
}

HAL_StatusTypeDef Init_ATK_ESP8266()
{
    // while (SendCmd_ATK_ESP8266("AT+RST", "OK", 20))
    //     ;
    // printf("resetting....\r\n");
    // HAL_Delay(3000);
    uint32_t i = 0;
    while (SendCmd_ATK_ESP8266("AT", "OK", 20))
    {
        i++;
        if (i % 30)
            printf("atk-esp8266 not online!\r\n");
    }
    printf("atk-esp8266 online\r\n");

    while (SendCmd_ATK_ESP8266("ATE0", "OK", 20))
        printf(atk_esp8266_rx_buff);

    while (SendCmd_ATK_ESP8266("AT+CWMODE=1", "OK", 20))
        printf(atk_esp8266_rx_buff);
    printf("set udp mode\r\n");

    char cmd[100];
    sprintf((char *)cmd, "AT+CWJAP=\"%s\",\"%s\"", wifi_ssid, wifi_password);
    while (SendCmd_ATK_ESP8266(cmd, "WIFI GOT IP", 5000))
        printf(atk_esp8266_rx_buff);
    printf("WIFI GOT IP\r\n");

    sprintf((char *)cmd, "AT+CIPSTART=\"%s\",\"%s\",%d,%d", "UDP", udp_remote_ip, udp_remote_port, udp_local_port);
    while (SendCmd_ATK_ESP8266(cmd, "OK", 50))
        printf(atk_esp8266_rx_buff);
    printf("connect to port:%d\r\n", udp_remote_port);

    while (SendCmd_ATK_ESP8266("AT+CIPMODE=1", "OK", 20))
        ;
    while (SendCmd_ATK_ESP8266("AT+CIPSEND", "OK", 20))
        ;
    printf("init success!\r\n");

    return HAL_OK;
}