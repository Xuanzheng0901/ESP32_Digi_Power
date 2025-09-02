#ifndef __ADS1115_REG_H
#define __ADS1115_REG_H

/************ CONFIG REGISTER: 16 bits ************/

/* Bit[15] OS: Operational Status / Start Conversion */
#define ADS1115_OS_NO_EFFECT 0x0000    // 读时：转换进行中；写时：无效（保留）
#define ADS1115_OS_START_SINGLE 0x8000 // 启动一次单次转换（写时设置）

/* Bits[14:12] MUX: Input multiplexer configuration */
#define ADS1115_MUX_DIFF_0_1 0x0000 // AINP = AIN0, AINN = AIN1 (默认差分)
#define ADS1115_MUX_DIFF_0_3 0x1000 // AINP = AIN0, AINN = AIN3
#define ADS1115_MUX_DIFF_1_3 0x2000 // AINP = AIN1, AINN = AIN3
#define ADS1115_MUX_DIFF_2_3 0x3000 // AINP = AIN2, AINN = AIN3
#define ADS1115_MUX_SINGLE_0 0x4000 // AIN0 - GND（单端）
#define ADS1115_MUX_SINGLE_1 0x5000 // AIN1 - GND（单端）
#define ADS1115_MUX_SINGLE_2 0x6000 // AIN2 - GND（单端）
#define ADS1115_MUX_SINGLE_3 0x7000 // AIN3 - GND（单端）

/* Bits[11:9] PGA: Programmable gain amplifier configuration (Full-Scale Range) */
#define ADS1115_PGA_6_144V 0x0000      // ±6.144V（需VDD ≥ 5V，LSB = 187.5μV）
#define ADS1115_PGA_4_096V 0x0200      // ±4.096V（LSB = 125μV）
#define ADS1115_PGA_2_048V 0x0400      // ±2.048V（默认，LSB = 62.5μV）
#define ADS1115_PGA_1_024V 0x0600      // ±1.024V（LSB = 31.25μV）
#define ADS1115_PGA_0_512V 0x0800      // ±0.512V（LSB = 15.625μV）
#define ADS1115_PGA_0_256V 0x0A00      // ±0.256V（LSB = 7.8125μV）
#define ADS1115_PGA_0_256V_ALT1 0x0C00 // 与 0x0A00 等价
#define ADS1115_PGA_0_256V_ALT2 0x0E00 // 与 0x0A00 等价

/* Bit[8] MODE: Device operating mode */
#define ADS1115_MODE_CONTINUOUS 0x0000  // 连续转换模式
#define ADS1115_MODE_SINGLE_SHOT 0x0100 // 单次转换模式（默认，省电）

/* Bits[7:5] Data Rate (SPS = samples per second) */
#define ADS1115_DR_8SPS 0x0000
#define ADS1115_DR_16SPS 0x0020
#define ADS1115_DR_32SPS 0x0040
#define ADS1115_DR_64SPS 0x0060
#define ADS1115_DR_128SPS 0x0080 // 默认
#define ADS1115_DR_250SPS 0x00A0
#define ADS1115_DR_475SPS 0x00C0
#define ADS1115_DR_860SPS 0x00E0

/* Bit[4] Comparator mode */
#define ADS1115_COMP_MODE_TRAD 0x0000   // 传统比较器（默认）
#define ADS1115_COMP_MODE_WINDOW 0x0010 // 窗口比较器

/* Bit[3] Comparator polarity */
#define ADS1115_COMP_POL_LOW 0x0000  // ALERT/RDY低电平有效（默认）
#define ADS1115_COMP_POL_HIGH 0x0008 // ALERT/RDY高电平有效

/* Bit[2] Comparator latch */
#define ADS1115_COMP_LAT_NON 0x0000   // 非锁存（默认）
#define ADS1115_COMP_LAT_LATCH 0x0004 // 锁存输出

/* Bits[1:0] Comparator queue & disable */
#define ADS1115_COMP_QUE_1CONV 0x0000   // 1次超限触发
#define ADS1115_COMP_QUE_2CONV 0x0001   // 2次超限触发
#define ADS1115_COMP_QUE_4CONV 0x0002   // 4次超限触发
#define ADS1115_COMP_QUE_DISABLE 0x0003 // 关闭比较器功能（推荐）

#endif