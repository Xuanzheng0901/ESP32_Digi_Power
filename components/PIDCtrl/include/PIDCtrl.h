#ifndef __CTRL_MAIN_H
#define __CTRL_MAIN_H

#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include <stdbool.h>
#include "PWMCtrl.h"
#include "ADCDetect.h"
#include "UIControl.h"
#include "pid_ctrl.h"
#include "esp_err.h"
#include <stdbool.h>

void pid_ctrl_init(void);

/**
 * @brief 设置目标电压
 * @param[in] mv 目标电压,单位mV
 */
void pid_set_voltage(uint32_t mv);

#endif