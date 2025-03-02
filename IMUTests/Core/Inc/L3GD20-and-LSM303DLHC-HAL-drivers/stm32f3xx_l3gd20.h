#ifndef __L3GD20_H__
#define __L3GD20_H__

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f3xx_hal.h"

/* default CS pin on STM32F3 Discovery board */
#define L3GD20_CS_PORT    GPIOE
#define L3GD20_CS_PIN     GPIO_PIN_3

/* pin macros */
#define L3GD20_CS_LOW     HAL_GPIO_WritePin(L3GD20_CS_PORT, L3GD20_CS_PIN, GPIO_PIN_RESET)
#define L3GD20_CS_HIGH    HAL_GPIO_WritePin(L3GD20_CS_PORT, L3GD20_CS_PIN, GPIO_PIN_SET)

/* identification number */
#define L3GD20_WHO_AM_I    0xD4

/* registers addresses */
#define L3GD20_REG_WHO_AM_I         0x0F
#define L3GD20_REG_CTRL_REG1        0x20
#define L3GD20_REG_CTRL_REG2        0x21
#define L3GD20_REG_CTRL_REG3        0x22
#define L3GD20_REG_CTRL_REG4        0x23
#define L3GD20_REG_CTRL_REG5        0x24
#define L3GD20_REG_REFERENCE        0x25
#define L3GD20_REG_OUT_TEMP         0x26
#define L3GD20_REG_STATUS_REG       0x27
#define L3GD20_REG_OUT_X_L          0x28
#define L3GD20_REG_OUT_X_H          0x29
#define L3GD20_REG_OUT_Y_L          0x2A
#define L3GD20_REG_OUT_Y_H          0x2B
#define L3GD20_REG_OUT_Z_L          0x2C
#define L3GD20_REG_OUT_Z_H          0x2D
#define L3GD20_REG_FIFO_CTRL_REG    0x2E
#define L3GD20_REG_FIFO_SRC_REG     0x2F
#define L3GD20_REG_INT1_CFG         0x30
#define L3GD20_REG_INT1_SRC         0x31
#define L3GD20_REG_INT1_TSH_XH      0x32
#define L3GD20_REG_INT1_TSH_XL      0x33
#define L3GD20_REG_INT1_TSH_YH      0x34
#define L3GD20_REG_INT1_TSH_YL      0x35
#define L3GD20_REG_INT1_TSH_ZH      0x36
#define L3GD20_REG_INT1_TSH_ZL      0x37
#define L3GD20_REG_INT1_DURATION    0x38

/* sensitivity factors, datasheet pg. 9 */
#define L3GD20_SENSITIVITY_250     8.75	// 8.75 mdps/digit
#define L3GD20_SENSITIVITY_500     17.5	// 17.5 mdps/digit
#define L3GD20_SENSITIVITY_2000    70   // 70 mdps/digit

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} l3gd20_data_t;

typedef enum {
    L3GD20_OK,
    L3GD20_ERROR
} l3gd20_result_t;

typedef enum {
    L3GD20_SCALE_250, // full scale to 250 mdps
    L3GD20_SCALE_500, // full scale to 500 mdps
    L3GD20_SCALE_2000 // full scale to 2000 mdps
} l3gd20_scale_t;


l3gd20_result_t l3gd20_init(SPI_HandleTypeDef *hspi, l3gd20_scale_t scale);
l3gd20_result_t l3gd20_read(l3gd20_data_t *data);

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

