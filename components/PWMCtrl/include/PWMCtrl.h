#ifndef __PWMCTRL_H
#define __PWMCTRL_H

#include "stdio.h"
#include "driver/ledc.h"

#define VSET_IO 9
#define ISET_IO 10

#define VSET_CHN 0
#define ISET_CHN 1

void pwm_init(void);
void set_pwm_duty(uint8_t channel, int32_t duty);

#endif