#ifndef __CONSOLE_H
#define __CONSOLE_H

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "PWMCtrl.h"

void console_init(void);

#endif