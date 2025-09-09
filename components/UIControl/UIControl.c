#include <stdio.h>
#include "UIControl.h"

extern lv_obj_t *highlight_frame;
extern lv_anim_t focus_anim;
extern void focus_event_cb(lv_event_t *e);

EventGroupHandle_t BootGroup = NULL;

static const char *TAG = "UI";
static lv_display_t *disp = NULL;
static lv_indev_t *indev = NULL;
lv_group_t *group = NULL;

static lv_obj_t *voltage_label = NULL;
static lv_obj_t *current_label = NULL;
static lv_obj_t *power_value_label = NULL;
lv_obj_t *voltage_spinbox = NULL;
lv_obj_t *current_spinbox = NULL;

static char value_buf[3][32] = {0};

void set_current(uint32_t mA)
{
    uint32_t duty = (uint32_t)((double)mA / 1000.0 * 0.005 * 50.0 + 0.625) / 3.3 * 2048.0;
    set_pwm_duty(ISET_CHN, duty);
}

static void lvgl_event_cb(lv_event_t *evt)
{
    int32_t value = lv_spinbox_get_value(lv_event_get_current_target(evt));
    ESP_LOGI(TAG, "Value changed: %ld", value);
    if (lv_event_get_current_target(evt) == voltage_spinbox)
        pid_set_voltage(value * 10);
    else
        set_current(value * 10);
} 

static void value_update_task(void* arg)
{
    while(1)
    {
        double voltage = ADS1115_read_channel(0);
        double current = ADS1115_read_channel(1);
        snprintf(value_buf[0], 6, "%5.2lf", voltage);
        snprintf(value_buf[1], 5, "%4.2lf", current);
        snprintf(value_buf[2], 8, "%6.2lfW", voltage * current);

        if(lvgl_port_lock(0))
        {
            lv_label_set_text_static(voltage_label, value_buf[0]);
            lv_label_set_text_static(current_label, value_buf[1]);
            lv_label_set_text_static(power_value_label, value_buf[2]);
            lvgl_port_unlock();
        }
        
        vTaskDelay(100);
    }
}

static void home_page_init(void)
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
        lv_label_set_text_static(voltage_label, value_buf[0]);
        lv_obj_set_width(voltage_label, 30);
        lv_obj_set_style_text_align(voltage_label, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_align(voltage_label, LV_ALIGN_TOP_LEFT, 37, 32);

        //电流数值
        current_label = lv_label_create(lv_screen_active());
        lv_label_set_text_static(current_label, value_buf[1]);
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
        lv_label_set_text(power_value_label, value_buf[2]);
        lv_obj_set_width(power_value_label, 42);
        lv_obj_set_style_text_align(power_value_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(power_value_label, LV_ALIGN_TOP_LEFT, 31, 47);


        //电压调整框
        voltage_spinbox = lv_spinbox_create(lv_screen_active());
        lv_spinbox_set_range(voltage_spinbox, 0, 2400);
        lv_spinbox_set_digit_format(voltage_spinbox, 4, 2);
        lv_spinbox_set_step(voltage_spinbox, 1);

        lv_obj_set_content_height(voltage_spinbox, 12);

        lv_obj_set_style_pad_all(voltage_spinbox, -1, 0);
        lv_obj_set_style_border_width(voltage_spinbox, 0, 0);

        lv_obj_set_size(voltage_spinbox, 32, 10);

        lv_obj_set_style_outline_opa(voltage_spinbox, 0, LV_STATE_FOCUS_KEY);

        lv_obj_set_style_bg_color(voltage_spinbox, lv_color_black(), LV_PART_CURSOR | LV_STATE_EDITED);
        lv_obj_set_style_text_color(voltage_spinbox, lv_color_white(), LV_PART_CURSOR | LV_STATE_EDITED);
        lv_obj_set_style_text_color(voltage_spinbox, lv_color_black(), LV_PART_CURSOR);

        lv_obj_set_style_text_align(voltage_spinbox, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_align(voltage_spinbox, LV_ALIGN_TOP_LEFT, 36, 18);
        lv_group_add_obj(group, voltage_spinbox);
        lv_obj_add_event_cb(voltage_spinbox, lvgl_event_cb, LV_EVENT_VALUE_CHANGED, NULL);


        //电流调整框
        current_spinbox = lv_spinbox_create(lv_screen_active());
        lv_spinbox_set_range(current_spinbox, 0, 2400);
        lv_spinbox_set_digit_format(current_spinbox, 3, 1);
        lv_spinbox_set_step(current_spinbox, 1);

        lv_obj_set_content_height(current_spinbox, 12);

        lv_obj_set_style_pad_all(current_spinbox, -1, 0);
        lv_obj_set_style_border_width(current_spinbox, 0, 0);

        lv_obj_set_size(current_spinbox, 26, 10);

        lv_obj_set_style_outline_opa(current_spinbox, 0, LV_STATE_FOCUS_KEY);

        lv_obj_set_style_bg_color(current_spinbox, lv_color_black(), LV_PART_CURSOR | LV_STATE_EDITED);
        lv_obj_set_style_text_color(current_spinbox, lv_color_white(), LV_PART_CURSOR | LV_STATE_EDITED);
        lv_obj_set_style_text_color(current_spinbox, lv_color_black(), LV_PART_CURSOR);

        lv_obj_set_style_text_align(current_spinbox, LV_TEXT_ALIGN_CENTER, 0);

        lv_obj_align(current_spinbox, LV_ALIGN_TOP_RIGHT, -9, 18);
        lv_group_add_obj(group, current_spinbox);
        lv_obj_add_event_cb(current_spinbox, lvgl_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

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
        lv_obj_add_event_cb(voltage_spinbox, focus_event_cb, LV_EVENT_FOCUSED, NULL);
        lv_obj_add_event_cb(current_spinbox, focus_event_cb, LV_EVENT_FOCUSED, NULL);

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
    // i2c_master_bus_handle_t i2c_bus = NULL;
    // i2c_master_bus_config_t bus_config = {
    //     .clk_source = I2C_CLK_SRC_DEFAULT,
    //     .glitch_ignore_cnt = 7,
    //     .i2c_port = I2C_NUM_0,
    //     .sda_io_num = 14,
    //     .scl_io_num = 13,
    //     .flags.enable_internal_pullup = true,
    // };
    // i2c_new_master_bus(&bus_config, &i2c_bus);

    // ESP_LOGI(TAG, "Install panel IO");
    // esp_lcd_panel_io_handle_t io_handle = NULL;
    // esp_lcd_panel_io_i2c_config_t io_config = {
    //     .dev_addr = 0x3C,
    //     .scl_speed_hz = 400 * 1000,
    //     .control_phase_bytes = 1,
    //     .lcd_cmd_bits = 8,
    //     .lcd_param_bits = 8,
    //     .dc_bit_offset = 6,
    // };
    // esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle);

    // ESP_LOGI(TAG, "Install SSD1306 panel driver");
    // esp_lcd_panel_handle_t panel_handle = NULL;
    // esp_lcd_panel_dev_config_t panel_config = {
    //     .bits_per_pixel = 1,
    //     .reset_gpio_num = -1,
    // };
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

void ui_setup(void* arg)
{
    lv_obj_t *label = NULL;
        BootGroup = xEventGroupCreate();

    UI_HW_Init();
    if(lvgl_port_lock(0))
    {
        label = lv_label_create(lv_screen_active());
        lv_label_set_text(label, "Initializing...");
        lv_obj_set_width(label, 90);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        lvgl_port_unlock();
    }
    indev_init();
    // xEventGroupWaitBits(BootGroup, ADS1115_INIT | PWM_INIT, pdTRUE, pdTRUE, portMAX_DELAY);
    if(lvgl_port_lock(0))
    {
        lv_obj_delete(label);
        lvgl_port_unlock();
    }
    
    home_page_init();
    xTaskCreate(value_update_task, "update value", 4096, NULL, 5, NULL);
    vTaskDelete(NULL);
}

void UI_Init(void)
{
    xTaskCreate(ui_setup, "UIBOOT", 4096, NULL, 5, NULL);
}