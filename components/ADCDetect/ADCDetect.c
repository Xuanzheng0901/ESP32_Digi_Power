#include "ADCDetect.h"
#include "freertos/task.h"
#include "esp_err.h"

static const char *TAG = "ADS1115";

i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;

void ADS1115_write(uint8_t addr, uint16_t data)
{
    uint8_t buffer[3] = {0};
    buffer[0] = addr;
    buffer[1] = (data >> 8) & 0xFF;
    buffer[2] = data & 0xFF;
    i2c_master_transmit(dev_handle, buffer, 3, 10);
}

void ADS1115_read_channel(uint8_t channel, int16_t* value)
{
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
    vTaskDelay(pdMS_TO_TICKS(10));

    static uint8_t read_buf[2];
    uint8_t addr = 0;
    i2c_master_transmit_receive(dev_handle, &addr, 1, read_buf, 2, 10);
    vTaskDelay(pdMS_TO_TICKS(10));

    *value = read_buf[0];
    *value = *value << 8;
    *value |= read_buf[1];
}

void ADS1115_Init(void)
{
    esp_err_t ret;
    
    i2c_master_bus_config_t bus_cfg = 
    {
        .clk_source                   = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
        .i2c_port                     = I2C_NUM_0, 
        .scl_io_num                   = 10,
        .sda_io_num                   = 11,
        .trans_queue_depth            = 16
    };
    ret = i2c_new_master_bus(&bus_cfg, &bus_handle);
    if (ret != ESP_OK) {
        printf("Failed to create I2C master bus: %s\n", esp_err_to_name(ret));
        return;
    }
    
    i2c_device_config_t ads1115_cfg = 
    {
        .dev_addr_length         = I2C_ADDR_BIT_LEN_7, 
        .device_address          = 0x48,
        .flags.disable_ack_check = false,
        .scl_speed_hz            = 400 * 1000
    };
    ret = i2c_master_bus_add_device(bus_handle, &ads1115_cfg, &dev_handle);
    if (ret != ESP_OK) {
        printf("Failed to add I2C device: %s\n", esp_err_to_name(ret));
        return;
    }

    uint16_t config = ADS1115_CONFIG_DEFAULT;
    uint8_t buffer[2] = {0};
    i2c_master_transmit_receive(dev_handle, (uint8_t*)&config, 2, buffer, 2, 10);
    vTaskDelay(pdMS_TO_TICKS(100));
    ADS1115_write(0x01, config);
}