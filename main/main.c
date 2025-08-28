#include "stdio.h"
#include "PWMCtrl.h"
#include "ConsoleCMD.h"
#include "ADCDetect.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "UIControl.h"
#include "esp_log.h"
#include "EC11.h"


void app_main(void)
{
    // EC11_init();3
    UI_Init();
    // pwm_init();
    // console_init();
    
    // // 初始化ADS1115
    // ADS1115_Init();

    // int16_t channel0_data = 0, channel1_data = 0;
    // while(1)
    // {
    //     ADS1115_read_channel(0, &channel0_data);
    //     ADS1115_read_channel(1, &channel1_data);

    //     double ch0_vol = channel0_data * 0.125 * 7 / 1000;
    //     double ch1_vol = channel1_data * 0.125 * 2 / 1.155;

    //     ESP_LOGI("ADS1115", "CH0 Voltage: %.2lfV", ch0_vol);
    //     ESP_LOGI("ADS1115", "CH1 Current: %.2lfmA\n", ch1_vol);
    //     vTaskDelay(pdMS_TO_TICKS(500));
    // }
}