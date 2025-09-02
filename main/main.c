#include "stdio.h"
#include "PWMCtrl.h"
#include "ConsoleCMD.h"
#include "ADCDetect.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "UIControl.h"
#include "esp_log.h"
#include "PIDCtrl.h"

void app_main(void)
{
    ADS1115_Init();
    pwm_init();
    pid_ctrl_init();
    UI_Init();
}