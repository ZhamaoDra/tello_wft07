#ifndef _PPM_DECODER_H
#define _PPM_DECODER_H

#include <stdio.h>
#include <stm32f1xx_hal.h>

#define PPM_DATA_MAX_SIZE 10

// define the connection of PPM here
// should modify all of thems
extern TIM_HandleTypeDef htim2;
#define PPM_TIM htim2
#define PPM_TIM_CHANNEL TIM_CHANNEL_1

typedef struct
{
    uint16_t time_now, time_last; // in ms
    uint16_t period;              // in ms
    int status;
    // 0 --- header of frame
    // 1 --- 1st channel
    // 2 --- 2nd channel
    // 3 --- 3rd channel
    // ....
    uint16_t data[PPM_DATA_MAX_SIZE]; // in ms
} PPM_TypeDef;

typedef struct
{
    uint8_t len; // the length of a frame
                 // also the range of the data array
                 // we can visit
    uint16_t *data;
} RC_TypeDef;

typedef struct
{
    TIM_HandleTypeDef *timer;
    uint32_t tim_channel;
    PPM_TypeDef ppm;
    RC_TypeDef *rc;
} PPM_TIM_Connection_TypeDef;

HAL_StatusTypeDef InitPPM(RC_TypeDef *rc, TIM_HandleTypeDef *tim, uint32_t channel);

#endif