#include <stdio.h>
#include "PWMCtrl.h"

void set_pwm_duty(uint8_t channel, int32_t duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
}

void pwm_init(void)
{
    ledc_timer_config_t timer_config = {
        .clk_cfg = LEDC_AUTO_CLK,
        .duty_resolution = LEDC_TIMER_11_BIT,
        .freq_hz = 20000,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
    };
    ledc_timer_config(&timer_config);

    ledc_channel_config_t channel_config = {
        .duty = 0,
        .flags.output_invert = 0,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
        .channel = 1,
        .gpio_num = VSET_IO
    };
    ledc_channel_config(&channel_config);

    channel_config.channel = 2;
    channel_config.gpio_num = ISET_IO;
    ledc_channel_config(&channel_config);
}