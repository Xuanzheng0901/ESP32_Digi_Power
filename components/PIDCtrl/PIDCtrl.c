#include "PIDCtrl.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "freertos/task.h"

#define LEDC_DUTY_MAX  (1 << LEDC_TIMER_11_BIT)

static const char *TAG_OUT = "PID";
static QueueHandle_t pid_set_queue = NULL;
static pid_ctrl_block_handle_t pid_handle = NULL;

static int calc_forward_duty(uint32_t target_mv) //根据前馈电压计算占空比
{
    float target_v = (float)target_mv / 1000.0f;
    if (target_v <= 0.6f) return 0;
    if (target_v >= 24.5f) return LEDC_DUTY_MAX;

    float x = (target_v < 9.65f) ? ((target_v + 0.0023f) / 0.0189f) : ((target_v - 0.6504f) / 0.0176f);
    if (x < 0.0f) x = 0.0f;
    if (x > (float)LEDC_DUTY_MAX) x = (float)LEDC_DUTY_MAX;

    return (int)(x + 0.5f);
}

// 根据目标电压(mV)计算 电压→占空比的比例 (duty/mV)
static float calc_slope(uint32_t target_mv)
{
    float target_v = (float)target_mv / 1000.0f;
    return (target_v < 9.65f) ? (1.0f / 0.0189f / 1000.0f) : (1.0f / 0.0176f / 1000.0f);
}

static inline uint32_t adc_raw_to_mV(int16_t raw)
{
    float mv = (float)raw * 0.125f * 7.0f; // 0.125mV/LSB * 分压 7 倍
    if (mv < 0) mv = 0;
    return (uint32_t)(mv + 0.5f);
}

static void control_task(void *arg)
{
    static int16_t forward_duty = 0;       // 前馈占空比
    static uint32_t exact_voltage_mV;
    static uint32_t set_buf;
    static float slope = 0.053f;           // dx/dy 斜率(duty/mV)
    static uint32_t target_voltage_mV = 0; // 默认 0V,不启动
    static float pid_calc_result_mV = 0.0f;
    while (1)
    {
        if(pdPASS == xQueueReceive(pid_set_queue, &set_buf, 0))
        {
            target_voltage_mV = set_buf;

            forward_duty = calc_forward_duty(set_buf);
            slope = calc_slope(set_buf);
            pid_reset_ctrl_block(pid_handle);
            set_pwm_duty(VSET_CHN, forward_duty);
            continue;
        }
        if(target_voltage_mV <= 600) continue;

        int16_t ADS1115_read_buf = ADS1115_read_channel(0);
        exact_voltage_mV = adc_raw_to_mV(ADS1115_read_buf);
        int32_t err_mV = (int32_t)target_voltage_mV - (int32_t)exact_voltage_mV;

        pid_compute(pid_handle, (float)err_mV, &pid_calc_result_mV);

        // 合成并限幅
        int duty = forward_duty + (int)(pid_calc_result_mV * slope);
        if (duty < 0) duty = 0;
        if (duty > LEDC_DUTY_MAX) duty = LEDC_DUTY_MAX;
        set_pwm_duty(VSET_CHN, duty);

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void pid_set_voltage(uint32_t mv)
{
    if(NULL == pid_set_queue) return;
    xQueueSend(pid_set_queue, &mv, portMAX_DELAY);   
}

void pid_ctrl_init(void)
{
    pid_ctrl_config_t pid_cfg = {
        .init_param = {
            .kp = 2.0f,
            .ki = 0.6f,
            .kd = 0.2f,
            .max_output = 6000.0f,   // mV
            .min_output = -6000.0f,  // mV
            .max_integral = 3000.0f, // mV
            .min_integral = -3000.0f,// mV
            .cal_type = PID_CAL_TYPE_POSITIONAL,
        }
    };
    pid_new_control_block(&pid_cfg, &pid_handle);
    pid_set_queue = xQueueCreate(8, sizeof(uint32_t));
    xTaskCreate(control_task, "PIDControl", 4096, NULL, 5, NULL);
    pid_set_voltage(0);
    ESP_LOGI(TAG_OUT, "PID started");
}