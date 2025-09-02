#ifndef __ADCD_H
#define __ADCD_H

#include "stdio.h"
#include "string.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"
#include "ADS1115_REG.h"

#define ADS1115_CONFIG_DEFAULT                                             \
    (ADS1115_OS_START_SINGLE | ADS1115_MUX_SINGLE_0 | ADS1115_PGA_4_096V | \
     ADS1115_MODE_CONTINUOUS | ADS1115_DR_250SPS | ADS1115_COMP_QUE_DISABLE)

#define ADS1115_SDA 17
#define ADS1115_SCL 18

void ADS1115_Init(void);
void ADS1115_read_channel(uint8_t channel, int16_t* value);


#endif