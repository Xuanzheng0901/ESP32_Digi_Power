#ifndef __PWMCTRL_H
#define __PWMCTRL_H

#include "stdio.h"
#include "driver/ledc.h"

void pwm_init(void);
void set_pwm_duty(int32_t duty);

#endif