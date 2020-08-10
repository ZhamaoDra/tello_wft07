#include "ppm_decoder.h"

#define PPM_STATUS_INITIAL 11
#define PPM_PERIOD_HEADER_MIN 5000
#define PPM_PERIOD_DATA_MAX 2500
#define PPM_PERIOD_DATA_MIN 500
#define LOOPMINUS(a, b, range) (a > b ? a - b : a + range - b)

PPM_TIM_Connection_TypeDef conn;

HAL_StatusTypeDef InitPPM(RC_TypeDef *rc, TIM_HandleTypeDef *tim, uint32_t channel)
{
    rc->len = 0;
    rc->data = conn.ppm.data;
    
    conn.rc = rc;
    conn.timer = tim;
    conn.tim_channel = channel;

    conn.ppm.period = 0;
    conn.ppm.time_now = 0;
    conn.ppm.time_last = 0;
    conn.ppm.status = PPM_STATUS_INITIAL;
    
    for (uint8_t i = 0; i < PPM_DATA_MAX_SIZE; i++)
        conn.ppm.data[i] = 0;
    
    if (HAL_TIM_IC_Start_IT(tim, channel))
    {
        printf("TIM input capture error!\r\n");
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef UpdatePPM()
{
    conn.ppm.time_now = HAL_TIM_ReadCapturedValue(conn.timer,conn.tim_channel);
    if(conn.ppm.time_last)
        conn.ppm.period = LOOPMINUS(conn.ppm.time_now,conn.ppm.time_last,conn.timer->Init.Period);
    conn.ppm.time_last = conn.ppm.time_now;

    if(conn.ppm.status != PPM_STATUS_INITIAL)
    {
        if(conn.ppm.period <= PPM_PERIOD_DATA_MAX && conn.ppm.period > PPM_PERIOD_DATA_MIN){
            conn.ppm.status++;
            conn.ppm.data[conn.ppm.status] = conn.ppm.period;
        }
        else if(conn.ppm.period > PPM_PERIOD_HEADER_MIN){
            conn.rc->len = conn.ppm.status;
            conn.ppm.status = 0;
            conn.ppm.data[conn.ppm.status] = conn.ppm.period;
        }
        else 
            conn.ppm.status = PPM_STATUS_INITIAL;
    }else
    {
        if(conn.ppm.period > PPM_PERIOD_HEADER_MIN){
            conn.ppm.status = 0;
            conn.ppm.data[conn.ppm.status] = conn.ppm.period;
        }
    }
    
    return HAL_OK;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == conn.timer->Instance && htim->Channel == conn.timer->Channel)
        UpdatePPM();
    else
        return;
}