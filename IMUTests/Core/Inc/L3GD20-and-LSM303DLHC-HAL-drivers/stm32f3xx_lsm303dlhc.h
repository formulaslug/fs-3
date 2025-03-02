#ifndef __LSM303DLHC_H__
#define __LSM303DLHC_H__

#include <stdint.h>
#include <stdbool.h>

#include "stm32f3xx_hal.h"

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/* i2c addresses */
#define LSM303DLHC_ADDR_ACC    0x32
#define LSM303DLHC_ADDR_MAG    0x3C

/* accelerometer registers */
#define LSM303DLHC_REG_ACC_CTRL_REG1_A        0x20
#define LSM303DLHC_REG_ACC_CTRL_REG2_A        0x21
#define LSM303DLHC_REG_ACC_CTRL_REG3_A        0x22
#define LSM303DLHC_REG_ACC_CTRL_REG4_A        0x23
#define LSM303DLHC_REG_ACC_CTRL_REG5_A        0x24
#define LSM303DLHC_REG_ACC_CTRL_REG6_A        0x25
#define LSM303DLHC_REG_ACC_REFERENCE_A        0x26
#define LSM303DLHC_REG_ACC_STATUS_REG_A       0x27
#define LSM303DLHC_REG_ACC_OUT_X_L_A          0x28
#define LSM303DLHC_REG_ACC_OUT_X_H_A          0x29
#define LSM303DLHC_REG_ACC_OUT_Y_L_A          0x2A
#define LSM303DLHC_REG_ACC_OUT_Y_H_A          0x2B
#define LSM303DLHC_REG_ACC_OUT_Z_L_A          0x2C
#define LSM303DLHC_REG_ACC_OUT_Z_H_A          0x2D
#define LSM303DLHC_REG_ACC_FIFO_CTRL_REG_A    0x2E
#define LSM303DLHC_REG_ACC_FIFO_SRC_REG_A     0x2F
#define LSM303DLHC_REG_ACC_INT1_CFG_A         0x30
#define LSM303DLHC_REG_ACC_INT1_SOURCE_A      0x31
#define LSM303DLHC_REG_ACC_INT1_THS_A         0x32
#define LSM303DLHC_REG_ACC_INT1_DURATION_A    0x33
#define LSM303DLHC_REG_ACC_INT2_CFG_A         0x34
#define LSM303DLHC_REG_ACC_INT2_SOURCE_A      0x35
#define LSM303DLHC_REG_ACC_INT2_THS_A         0x36
#define LSM303DLHC_REG_ACC_INT2_DURATION_A    0x37
#define LSM303DLHC_REG_ACC_CLICK_CFG_A        0x38
#define LSM303DLHC_REG_ACC_CLICK_SRC_A        0x39
#define LSM303DLHC_REG_ACC_CLICK_THS_A        0x3A
#define LSM303DLHC_REG_ACC_TIME_LIMIT_A       0x3B
#define LSM303DLHC_REG_ACC_TIME_LATENCY_A     0x3C
#define LSM303DLHC_REG_ACC_TIME_WINDOW_A      0x3D

/* accelerometer CTRL_REG1_A */
#define LSM303DLHC_ACR1A_XEN                 (1 << 0)   // X axis enable. Default value: 1
#define LSM303DLHC_ACR1A_YEN                 (1 << 1)   // Y axis enable. Default value: 1
#define LSM303DLHC_ACR1A_ZEN                 (1 << 2)   // Z axis enable. Default value: 1
#define LSM303DLHC_ACR1A_LPEN                (1 << 3)   // Low-power mode enable. Default value: 0
#define LSM303DLHC_ACR1A_ODR30_POWER_DOWN    (0b0000 << 4)   // Power-down mode
#define LSM303DLHC_ACR1A_ODR30_1_HZ          (0b0001 << 4)   // Normal / low-power mode (1 Hz)
#define LSM303DLHC_ACR1A_ODR30_10_HZ         (0b0010 << 4)
#define LSM303DLHC_ACR1A_ODR30_25_HZ         (0b0011 << 4)
#define LSM303DLHC_ACR1A_ODR30_50_HZ         (0b0100 << 4)
#define LSM303DLHC_ACR1A_ODR30_100_HZ        (0b0101 << 4)
#define LSM303DLHC_ACR1A_ODR30_200_HZ        (0b0110 << 4)
#define LSM303DLHC_ACR1A_ODR30_400_HZ        (0b0111 << 4)
#define LSM303DLHC_ACR1A_ODR30_1620_HZ       (0b1000 << 4)
#define LSM303DLHC_ACR1A_ODR30_5376_HZ       (0b1001 << 4)   // Normal (1.344 kHz) / low-power mode (5.376 KHz)

/* accelerometer CTRL_REG4_A */
#define LSM303DLHC_ACR4A_SIM                 (1 << 0)       // SPI serial interface mode selection. Default value: 0
#define LSM303DLHC_ACR4A_HR                  (1 << 3)       // High resolution output mode: Default value: 0
#define LSM303DLHC_ACR4A_FS10_1MG            (0b00 << 4)    // 1 mg/LSB, +- 2G
#define LSM303DLHC_ACR4A_FS10_2MG            (0b01 << 4)    // 2 mg/LSB, +- 4G
#define LSM303DLHC_ACR4A_FS10_4MG            (0b10 << 4)    // 4 mg/LSB, +- 8G
#define LSM303DLHC_ACR4A_FS10_12MG           (0b11 << 4)    // 12 mg/LSB, +- 16G
#define LSM303DLHC_ACR4A_BLE                 (1 << 6)       // Big/little endian data selection. Default value 0
#define LSM303DLHC_ACR4A_BLU                 (1 << 7)       // Block data update. Default value: 0

/* accelerometer read byte order if LSM303DLHC_ACR4A_BLE = 0  */
#define LSM303DLHC_ACC_XLO   0
#define LSM303DLHC_ACC_XHI   1
#define LSM303DLHC_ACC_YLO   2
#define LSM303DLHC_ACC_YHI   3
#define LSM303DLHC_ACC_ZLO   4
#define LSM303DLHC_ACC_ZHI   5

#define LSM303DLHC_ACC_SENSORS_GRAVITY_STANDARD    (9.80665f)  // Earth's gravity in m/s^2

/* magnetometer registers  */
#define LSM303DLHC_REG_MAG_CRA_REG_M       0x00
#define LSM303DLHC_REG_MAG_CRB_REG_M       0x01
#define LSM303DLHC_REG_MAG_MR_REG_M        0x02
#define LSM303DLHC_REG_MAG_OUT_X_H_M       0x03
#define LSM303DLHC_REG_MAG_OUT_X_L_M       0x04
#define LSM303DLHC_REG_MAG_OUT_Z_H_M       0x05
#define LSM303DLHC_REG_MAG_OUT_Z_L_M       0x06
#define LSM303DLHC_REG_MAG_OUT_Y_H_M       0x07
#define LSM303DLHC_REG_MAG_OUT_Y_L_M       0x08
#define LSM303DLHC_REG_MAG_SR_REG_Mg       0x09
#define LSM303DLHC_REG_MAG_IRA_REG_M       0x0A
#define LSM303DLHC_REG_MAG_IRB_REG_M       0x0B
#define LSM303DLHC_REG_MAG_IRC_REG_M       0x0C
#define LSM303DLHC_REG_MAG_TEMP_OUT_H_M    0x31
#define LSM303DLHC_REG_MAG_TEMP_OUT_L_M    0x32

/* MR_REG_M */
typedef enum {
    LSM303DLHC_MAGOP_CONT = 0x00,   // Continuous-conversion mode
    LSM303DLHC_MAGOP_SINGLE = 0x01, // Single-conversion mode
    LSM303DLHC_MAGOP_SLEEP1 = 0x02, // Sleep-mode. Device is placed in sleep-mode
    LSM303DLHC_MAGOP_SLEEP2 = 0x03  // Sleep-mode. Device is placed in sleep-mode
} lsm303dlhc_mag_op_t;

/* CRA_REG_M */
typedef enum {
    LSM303DLHC_MAGRATE_0_7 = 0x00,  // 0.75 Hz
    LSM303DLHC_MAGRATE_1_5 = 0x01,  // 1.5 Hz
    LSM303DLHC_MAGRATE_3_0 = 0x62,  // 3.0 Hz
    LSM303DLHC_MAGRATE_7_5 = 0x03,  // 7.5 Hz
    LSM303DLHC_MAGRATE_15 = 0x04,  // 15 Hz
    LSM303DLHC_MAGRATE_30 = 0x05,  // 30 Hz
    LSM303DLHC_MAGRATE_75 = 0x06,  // 75 Hz
    LSM303DLHC_MAGRATE_220 = 0x07   // 200 Hz
} lsm303dlhc_mag_rate_t;

/* CRB_REG_M */
typedef enum {
    LSM303DLHC_MAGGAIN_1_3 = 0x20,  // +/- 1.3
    LSM303DLHC_MAGGAIN_1_9 = 0x40,  // +/- 1.9
    LSM303DLHC_MAGGAIN_2_5 = 0x60,  // +/- 2.5
    LSM303DLHC_MAGGAIN_4_0 = 0x80,  // +/- 4.0
    LSM303DLHC_MAGGAIN_4_7 = 0xA0,  // +/- 4.7
    LSM303DLHC_MAGGAIN_5_6 = 0xC0,  // +/- 5.6
    LSM303DLHC_MAGGAIN_8_1 = 0xE0  // +/- 8.1
} lsm303dlhc_mag_gain_t;

/* high before low (different than accel) */
#define LSM303DLHC_MAG_XHI   0
#define LSM303DLHC_MAG_XLO   1
#define LSM303DLHC_MAG_ZHI   2
#define LSM303DLHC_MAG_ZLO   3
#define LSM303DLHC_MAG_YHI   4
#define LSM303DLHC_MAG_YLO   5

#define LSM303DLHC_MAG_SENSORS_GAUSS_TO_MICROTESLA    (100.0f)  // Gauss to micro-Tesla multiplier

typedef enum {
    LSM303DLHC_OK, LSM303DLHC_ERROR
} lsm303dlhc_result_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} lsm303dlhc_data_raw_t;

typedef struct {
    float x;
    float y;
    float z;
} lsm303dlhc_data_t;

typedef struct {
    uint8_t ctrl_reg1_a;
    uint8_t ctrl_reg2_a;
    uint8_t ctrl_reg3_a;
    uint8_t ctrl_reg4_a;
    uint8_t ctrl_reg5_a;
    uint8_t ctrl_reg6_a;
} lsm303dlhc_acc_init_t;

typedef struct {
    lsm303dlhc_mag_op_t op;
    lsm303dlhc_mag_rate_t rate;
    lsm303dlhc_mag_gain_t gain;
    bool auto_range;
} lsm303dlhc_mag_init_t;

lsm303dlhc_result_t lsm303dlhc_init_acc(I2C_HandleTypeDef *i2c, const lsm303dlhc_acc_init_t *init);
lsm303dlhc_result_t lsm303dlhc_set_acc_scale(uint8_t ctrl_reg4_a);
lsm303dlhc_result_t lsm303dlhc_read_acc_raw(lsm303dlhc_data_raw_t *data);
void lsm303dlhc_convert_acc(lsm303dlhc_data_t *conv, const lsm303dlhc_data_raw_t *raw);

lsm303dlhc_result_t lsm303dlhc_init_mag(I2C_HandleTypeDef *i2c, const lsm303dlhc_mag_init_t *init);
lsm303dlhc_result_t lsm303dlhc_set_mag_gain(lsm303dlhc_mag_gain_t gain);
lsm303dlhc_result_t lsm303dlhc_set_mag_rate(lsm303dlhc_mag_rate_t rate);
lsm303dlhc_result_t lsm303dlhc_read_mag_raw(lsm303dlhc_data_raw_t *data);
void lsm303dlhc_convert_mag(lsm303dlhc_data_t *conv, const lsm303dlhc_data_raw_t *raw);

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif //__LSM303DLHC_H
