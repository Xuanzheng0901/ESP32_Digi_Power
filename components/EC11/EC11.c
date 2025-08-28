#include "EC11.h"

static const char* TAG = "EC11";
static knob_handle_t knob_handle = NULL;
button_handle_t btn_handle = NULL;

static void knob_cb(void *arg, void *data)
{
    static bool s_is_rotated = false;
    static int8_t direction = -1;

    // knob_handle_t h = (knob_handle_t)arg;
    knob_event_t evt = iot_knob_get_event((knob_handle_t)arg);
    if(evt != 0 && evt != 1) return;

    if(!s_is_rotated)
    {
        s_is_rotated = !s_is_rotated;
        direction = evt;
    }
    else
    {
        if(evt == direction)
        {
            ESP_LOGI(TAG, "%s", direction == 0 ? "LEFT" : "RIGHT");
        }
        s_is_rotated = false;
        direction = -1;
    }
}

static void button_event_cb(void *arg, void *data)
{
    iot_button_print_event((button_handle_t)arg);
}

void knob_init(void)
{
    knob_config_t cfg = {
        .default_direction = 0,
        .gpio_encoder_a = 0,
        .gpio_encoder_b = 2
    };
    knob_handle = iot_knob_create(&cfg);

    iot_knob_register_cb(knob_handle, KNOB_LEFT, knob_cb, NULL);
    iot_knob_register_cb(knob_handle, KNOB_RIGHT, knob_cb, NULL);
}

void button_init(void)
{
    button_config_t btn_cfg = {
        .long_press_time = 65535,
        .short_press_time = 150,
    };
    button_gpio_config_t io_cfg = {
        .active_level = 0,
        .disable_pull = 0,
        .gpio_num = 21
    };

    iot_button_new_gpio_device(&btn_cfg, &io_cfg, &btn_handle);
    // iot_button_register_cb(btn_handle, BUTTON_SINGLE_CLICK, NULL, button_event_cb, NULL);
    // iot_button_register_cb(btn_handle, BUTTON_DOUBLE_CLICK, NULL, button_event_cb, NULL);
}

void EC11_init(void)
{
    // knob_init();
    button_init();
}