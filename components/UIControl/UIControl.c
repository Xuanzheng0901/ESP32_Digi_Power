#include <stdio.h>
#include "UIControl.h"

extern lv_obj_t *highlight_frame;
extern lv_anim_t focus_anim;
extern void focus_event_cb(lv_event_t *e);

static const char *TAG = "UI";
static lv_display_t *disp = NULL;
static lv_indev_t *indev = NULL;
lv_group_t *group = NULL;

static lv_obj_t *voltage_label = NULL;
static lv_obj_t *current_label = NULL;
static lv_obj_t *power_value_label = NULL;
lv_obj_t *spinbox = NULL;
lv_obj_t *spinbox1 = NULL;

void home_page_init(void)
{
    if (lvgl_port_lock(0))
    {
        lv_obj_t *label1 = lv_label_create(lv_screen_active());
        lv_label_set_text(label1, "电压(V)  电流(A)");
        lv_obj_set_width(label1, 128);
        lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_align(label1, LV_ALIGN_TOP_LEFT, 0, 0);

        lv_obj_t *label2 = lv_label_create(lv_screen_active());
        lv_label_set_text(label2, "设定");
        lv_obj_set_width(label2, 24);
        lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label2, LV_ALIGN_TOP_LEFT, 0, 16);

        lv_obj_t *label3 = lv_label_create(lv_screen_active());
        lv_label_set_text(label3, "实际");
        lv_obj_set_width(label3, 24);
        lv_obj_set_style_text_align(label3, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label3, LV_ALIGN_TOP_LEFT, 0, 31);

        // 电压数值
        voltage_label = lv_label_create(lv_screen_active());
        lv_label_set_text(voltage_label, "12.05");
        lv_obj_set_width(voltage_label, 30);
        lv_obj_set_style_text_align(voltage_label, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_align(voltage_label, LV_ALIGN_TOP_LEFT, 37, 32);

        //电流数值
        current_label = lv_label_create(lv_screen_active());
        lv_label_set_text(current_label, "1.28");
        lv_obj_set_width(current_label, 24);
        lv_obj_set_style_text_align(current_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(current_label, LV_ALIGN_TOP_LEFT, 94, 32);

        lv_obj_t *power_label = lv_label_create(lv_screen_active());
        lv_label_set_text(power_label, "功率");
        lv_obj_set_width(power_label, 42);
        lv_obj_set_style_text_align(power_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(power_label, LV_ALIGN_TOP_LEFT, 0, 47);

        //功率数值
        power_value_label = lv_label_create(lv_screen_active());
        lv_label_set_text(power_value_label, "100.00W");
        lv_obj_set_width(power_value_label, 42);
        lv_obj_set_style_text_align(power_value_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(power_value_label, LV_ALIGN_TOP_LEFT, 31, 47);


        //电压调整框
        spinbox = lv_spinbox_create(lv_screen_active());
        lv_spinbox_set_range(spinbox, 0, 2400);
        lv_spinbox_set_digit_format(spinbox, 4, 2);
        lv_spinbox_set_step(spinbox, 1);

        lv_obj_set_content_height(spinbox, 12);

        lv_obj_set_style_pad_all(spinbox, -1, 0);
        lv_obj_set_style_border_width(spinbox, 0, 0);

        lv_obj_set_size(spinbox, 32, 10);

        lv_obj_set_style_outline_opa(spinbox, 0, LV_STATE_FOCUS_KEY);

        lv_obj_set_style_bg_color(spinbox, lv_color_black(), LV_PART_CURSOR | LV_STATE_EDITED);
        lv_obj_set_style_text_color(spinbox, lv_color_white(), LV_PART_CURSOR | LV_STATE_EDITED);
        lv_obj_set_style_text_color(spinbox, lv_color_black(), LV_PART_CURSOR);

        lv_obj_set_style_text_align(spinbox, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_align(spinbox, LV_ALIGN_TOP_LEFT, 36, 18);
        lv_group_add_obj(group, spinbox);


        //电流调整框
        spinbox1 = lv_spinbox_create(lv_screen_active());
        lv_spinbox_set_range(spinbox1, 0, 2400);
        lv_spinbox_set_digit_format(spinbox1, 3, 1);
        lv_spinbox_set_step(spinbox1, 1);

        lv_obj_set_content_height(spinbox1, 12);

        lv_obj_set_style_pad_all(spinbox1, -1, 0);
        lv_obj_set_style_border_width(spinbox1, 0, 0);

        lv_obj_set_size(spinbox1, 26, 10);

        lv_obj_set_style_outline_opa(spinbox1, 0, LV_STATE_FOCUS_KEY);

        lv_obj_set_style_bg_color(spinbox1, lv_color_black(), LV_PART_CURSOR | LV_STATE_EDITED);
        lv_obj_set_style_text_color(spinbox1, lv_color_white(), LV_PART_CURSOR | LV_STATE_EDITED);
        lv_obj_set_style_text_color(spinbox1, lv_color_black(), LV_PART_CURSOR);

        lv_obj_set_style_text_align(spinbox1, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_align(spinbox1, LV_ALIGN_TOP_RIGHT, -9, 18);
        lv_group_add_obj(group, spinbox1);

        // 创建焦点高亮框
        highlight_frame = lv_obj_create(lv_screen_active());
        lv_obj_set_size(highlight_frame, 35, 14);
        lv_obj_align(highlight_frame, LV_ALIGN_TOP_LEFT, 34, 16);
        
        // 设置高亮框样式
        lv_obj_set_style_bg_opa(highlight_frame, LV_OPA_TRANSP, 0); // 背景透明，不遮挡内容
        lv_obj_set_style_border_width(highlight_frame, 1, 0);
        lv_obj_set_style_border_color(highlight_frame, lv_color_black(), 0); // 黑色边框，适合白色背景
        lv_obj_set_style_border_opa(highlight_frame, LV_OPA_80, 0);
        lv_obj_set_style_radius(highlight_frame, 4, 0); // 增加圆角
        lv_obj_set_style_pad_all(highlight_frame, 0, 0);
        
        // 设置高亮框不可点击，避免干扰交互，并移到后层避免遮挡
        lv_obj_add_flag(highlight_frame, LV_OBJ_FLAG_FLOATING);
        lv_obj_clear_flag(highlight_frame, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_move_to_index(highlight_frame, 0); // 移到最底层
        
        // 为spinbox控件添加焦点事件
        lv_obj_add_event_cb(spinbox, focus_event_cb, LV_EVENT_FOCUSED, NULL);
        lv_obj_add_event_cb(spinbox1, focus_event_cb, LV_EVENT_FOCUSED, NULL);

        lvgl_port_unlock();
    }
}

void indev_init(void)
{
    button_handle_t btn_handle = NULL;
    button_config_t btn_cfg = {0};
    button_gpio_config_t io_cfg = {
        .active_level = 0,
        .disable_pull = false,
        .gpio_num = BTN_IO
    };
    iot_button_new_gpio_device(&btn_cfg, &io_cfg, &btn_handle);

    knob_config_t knob_cfg = {
        .default_direction = 0,
        .gpio_encoder_a = KNOB_A,
        .gpio_encoder_b = KNOB_B
    };
    lvgl_port_encoder_cfg_t cfg = {
        .disp = disp,
        .encoder_a_b = &knob_cfg,
        .encoder_enter = btn_handle
    };

    indev = lvgl_port_add_encoder(&cfg);
    group = lv_group_create();
    lv_indev_set_group(indev, group);
}

void UI_HW_Init(void)
{
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = OLED_SDA,
        .sclk_io_num = OLED_SCL,
        .data1_io_num = -1,
        .data2_io_num = -1,
        .data3_io_num = -1,
        .max_transfer_sz = 128 * 8,
    };
    spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_cfg = {
        .cs_gpio_num = OLED_CS,
        .dc_gpio_num = OLED_DC,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .pclk_hz = 500000,
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
        .reset_gpio_num = OLED_RST,
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
        }
    };
    disp = lvgl_port_add_disp(&disp_cfg);
    
}

void UI_Init(void)
{
    UI_HW_Init();
    indev_init();
    home_page_init();
}