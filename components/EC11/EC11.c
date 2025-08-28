#include "EC11.h"

static const char* TAG = "EC11";
knob_handle_t knob_handle = NULL;

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
    
}

void EC11_init(void)
{

}