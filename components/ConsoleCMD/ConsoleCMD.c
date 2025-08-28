#include "ConsoleCMD.h"

#define BUF_SIZE 1024
#define MAX_CMD_LEN 256
#define PROMPT_STR "ESP32@localhost "

static const char* TAG = "Console";

static void process_command(char* cmd)
{
    char* newline = strchr(cmd, '\n');
    if (newline) *newline = '\0';
    
    char* carriage = strchr(cmd, '\r');
    if (carriage) *carriage = '\0';

    if (strlen(cmd) == 0)
        return;

    // 复制命令字符串用于解析
    char cmd_copy[MAX_CMD_LEN];
    strncpy(cmd_copy, cmd, MAX_CMD_LEN - 1);
    cmd_copy[MAX_CMD_LEN - 1] = '\0';

    // 解析命令和参数
    char* command = strtok(cmd_copy, " \t");
    if (command == NULL)
        return;

    if (strcmp(command, "setpwm") == 0)
    {
        char* param = strtok(NULL, " \t");
        if (param == NULL)
        {
            printf("错误: setpwm 命令需要一个参数\n");
            printf("用法: setpwm <值> (0.00-30.00)\n");
            return;
        }

        // 检查参数是否为有效的浮点数
        char* endptr;
        uint32_t duty = strtol(param, &endptr, 10);
        
        if (*endptr != '\0')
        {
            printf("错误: '%s' 不是有效的数字\n", param);
            return;
        }
        
        // if (value < 0.0 || value > 30.0)
        // {
        //     printf("错误: PWM值必须在0.00-30.00范围内\n");
        //     return;
        // }

        // printf("设置电压值为: %.2lf\n", value);
        // if(value < 9.6)
        // {
        //     duty = (uint32_t)((value + 0.0023) / 0.0094);
        // }
        // else
        // {
        //     duty = (uint32_t)((value - 0.6504) / 0.0088);
        // }
        set_pwm_duty(duty);
    }
    else if (strcmp(command, "help") == 0)
    {
        printf("可用命令:\n");
        printf("  help           - 显示帮助信息\n");
        printf("  setpwm <值>    - 设置PWM值 (0.00-30.00)\n");
    }
    else
    {
        printf("未知命令: %s\n", command);
        printf("输入 'help' 查看可用命令\n");
    }
}

static void uart_task(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(20));
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
    vTaskDelay(pdMS_TO_TICKS(20));

    ESP_LOGI(TAG, "UART已初始化");
    
    char cmd_buffer[MAX_CMD_LEN];
    int cmd_pos = 0;
    
    printf("\n");
    printf("=== ESP32 控制台框架 ===\n");
    printf("输入 'help' 查看可用命令\n");
    printf("%s> ", PROMPT_STR);
    fflush(stdout);
    
    while (1)
    {
        uint8_t data;
        int len = uart_read_bytes(UART_NUM_0, &data, 1, pdMS_TO_TICKS(100));
        if(len > 0)
        {
            char c = (char)data;

            if (c == '\b' || c == 127)
            {
                if (cmd_pos > 0)
                {
                    cmd_pos--;
                    printf("\b \b");
                    fflush(stdout);
                }
                continue;
            }

            printf("%c", c);
            fflush(stdout);

            if (c == '\r' || c == '\n')
            {
                cmd_buffer[cmd_pos] = '\0';
                printf("\n");
                
                process_command(cmd_buffer);
                
                cmd_pos = 0;
                printf("%s> ", PROMPT_STR);
                fflush(stdout);
            }
            else if (cmd_pos < MAX_CMD_LEN - 1)
            {
                cmd_buffer[cmd_pos++] = c;
            }
        }
    }
}

void console_init(void)
{
    xTaskCreate(uart_task, "uart_task", 4096, NULL, 5, NULL);
}
