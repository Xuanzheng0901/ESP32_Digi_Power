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
#include "ADCDetect.h"

#define PWM_INIT      1ULL << 0
#define ADS1115_INIT  1ULL << 1

#define OLED_SCL 13
#define OLED_SDA 14
#define OLED_RST 21
#define OLED_DC  47
#define OLED_CS  48

#define KNOB_A   2
#define KNOB_B   1
#define BTN_IO   0

#include "PIDCtrl.h"

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