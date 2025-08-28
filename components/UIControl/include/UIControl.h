#ifndef __UICTRL_H
#define __UICTRL_H

#include "esp_lcd_panel_io.h"
#include "driver/spi_master.h"
#include "lvgl.h"
#include "esp_lcd_panel_vendor.h"
#include "iot_button.h"
#include "button_gpio.h"
#include "iot_knob.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "stdio.h"

// 动画配置结构
typedef struct
{
    lv_coord_t spinbox_x;
    lv_coord_t spinbox_width;
    lv_coord_t spinbox1_x;
    lv_coord_t spinbox1_width;
} focus_anim_config_t;

void UI_Init(void);

#endif