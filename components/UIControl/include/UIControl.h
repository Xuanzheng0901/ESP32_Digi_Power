#ifndef __UICTRL_H
#define __UICTRL_H


#include "esp_lcd_panel_io.h"
// #include "driver/i2c_master.h"
#include "driver/spi_master.h"
#include "lvgl.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lvgl_port.h"

#include "esp_log.h"
#include "stdio.h"

void UI_Init(void);

#define I2C_SCL_NUM 18
#define I2C_SDA_NUM 8

#endif