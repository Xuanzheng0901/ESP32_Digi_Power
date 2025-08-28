#include <stdio.h>
#include "UIControl.h"

static const char* TAG = "UI";

void home_page_init(void)
{
    static char buf[64] = "Hello World";
    if (lvgl_port_lock(0))
    {
        lv_obj_t *voltage_label = lv_label_create(lv_screen_active());
        lv_label_set_text(voltage_label, "电压:12.51V");
        lv_obj_set_width(voltage_label, 128);
        lv_obj_set_style_text_align(voltage_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(voltage_label, LV_ALIGN_TOP_LEFT, 0, 0);

        lv_obj_t *current_label = lv_label_create(lv_screen_active());
        lv_label_set_text(current_label, "电流:1.22A");
        lv_obj_set_width(current_label, 128);
        lv_obj_set_style_text_align(current_label, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_align(current_label, LV_ALIGN_TOP_LEFT, 0, 0);

        lv_obj_t *power_label = lv_label_create(lv_screen_active());
        lv_label_set_text(power_label, "功率:100.55W");
        lv_obj_set_width(power_label, 128);
        lv_obj_set_style_text_align(power_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(power_label, LV_ALIGN_TOP_LEFT, 0, 13);
        lvgl_port_unlock();
    }
}

void UI_HW_Init(void)
{
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = 10,
        .sclk_io_num = 9,
        .data1_io_num = -1,
        .data2_io_num = -1,
        .data3_io_num = -1,
        .max_transfer_sz = 128 * 8,
    };
    spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_cfg = {
        .cs_gpio_num = 13,
        .dc_gpio_num = 12,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .pclk_hz = 1000000,
        .trans_queue_depth = 32,
        .spi_mode = 0,
        .flags = {
            .cs_high_active = 0,
            .dc_high_on_cmd = 0,
            .dc_low_on_data = 0,
            .dc_low_on_param = 1,
            .octal_mode = 0,
            .quad_mode = 0,
        }
    };
    esp_lcd_new_panel_io_spi(SPI2_HOST, &io_cfg, &io_handle);

    ESP_LOGD(TAG, "Install SSD1306 panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = 11,
    };
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = 64,
    };
    panel_config.vendor_config = &ssd1306_config;
    esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle);

    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_disp_on_off(panel_handle, true);

    ESP_LOGD(TAG, "Initialize LVGL");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = 128 * 64,
        .double_buffer = true,
        .hres = 128,
        .vres = 64,
        .monochrome = true,
        .color_format = LV_COLOR_FORMAT_RGB565,
        .rotation = {
            .swap_xy = false,
            .mirror_x = true,
            .mirror_y = true,
        }};
    // lv_disp_t *disp =
    lvgl_port_add_disp(&disp_cfg);
    
}

void UI_Init(void)
{
    UI_HW_Init();
    home_page_init();
}