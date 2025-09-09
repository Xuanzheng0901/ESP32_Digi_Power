#include "ADCDetect.h"
#include "freertos/task.h"
#include "esp_err.h"

static const char *TAG = "ADS1115";

static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t dev_handle;
static volatile double ADS1115_voltage[2];

static void ADS1115_write(uint8_t addr, uint16_t data)
{
    uint8_t buffer[3] = {0};
    buffer[0] = addr;
    buffer[1] = (data >> 8) & 0xFF;
    buffer[2] = data & 0xFF;
    ESP_ERROR_CHECK(i2c_master_transmit(dev_handle, buffer, 3, 10));
}

static void _ADS1115_read_channel(uint8_t channel)
{
    // static uint8_t last_read_channel = 1;
    // if(channel != last_read_channel)
    // {
        uint16_t buf_cfg = ADS1115_CONFIG_DEFAULT & (uint16_t)0x8FFF;
        switch (channel)
        {
            case 0:
                buf_cfg |= ADS1115_MUX_SINGLE_0;
                break;
            case 1:
                buf_cfg |= ADS1115_MUX_SINGLE_1;
                break;
            default:
                break;
        }
        ADS1115_write(0x01, buf_cfg);
        vTaskDelay(pdMS_TO_TICKS(5));
    // }

    static uint8_t read_buf[2];
    uint8_t addr = 0;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, &addr, 1, read_buf, 2, 10));
    // if(pdPASS != )
    //     return;

    int16_t value = (read_buf[0] << 8) | read_buf[1];
    if(channel)
    {
        ADS1115_voltage[!channel] = (double)value * 4.096 * 7.0 / 32768.0;
    }
    else
    {
        ADS1115_voltage[!channel] = ((double)value * 4.096 / 32768.0 - 0.60) / 50.0 / 0.005;
    }
    // ESP_LOGI(TAG, "channel: %d, value: %lf", channel, ADS1115_voltage[!channel]);
    // last_read_channel = channel;
}

static void ADS1115_read_task(void* arg)
{
    static uint8_t channel = 0;
    while(1)
    {
        // ESP_LOGI(TAG, "ADS1115 Detecting");
        _ADS1115_read_channel(channel);
        channel = !channel;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

double ADS1115_read_channel(uint8_t channel)
{
    return ADS1115_voltage[channel];
}

void ADS1115_Init(void)
{
    i2c_master_bus_config_t bus_cfg = {
        .clk_source                   = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
        .i2c_port                     = I2C_NUM_1, 
        .scl_io_num                   = ADS1115_SCL,
        .sda_io_num                   = ADS1115_SDA,
        .trans_queue_depth            = 4
    };
    i2c_new_master_bus(&bus_cfg, &bus_handle);
    
    i2c_device_config_t ads1115_cfg = {
        .dev_addr_length         = I2C_ADDR_BIT_LEN_7, 
        .device_address          = 0x48,
        .flags.disable_ack_check = false,
        .scl_speed_hz            = 400 * 1000
    };
    i2c_master_bus_add_device(bus_handle, &ads1115_cfg, &dev_handle);

    xTaskCreate(ADS1115_read_task, "ADS1115", 8192, NULL, 5, NULL);
}