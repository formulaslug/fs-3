#include "stm32f3xx_l3gd20.h"

/* private variables */
static l3gd20_scale_t l3gd20_scale;
static SPI_HandleTypeDef *l3gd20_hspi = NULL;

/* private functions */
static l3gd20_result_t l3gd20_read_spi(uint8_t address, uint8_t *data);
static l3gd20_result_t l3gd20_write_spi(uint8_t address, uint8_t data);

l3gd20_result_t l3gd20_init(SPI_HandleTypeDef *hspi, l3gd20_scale_t scale) {
    l3gd20_hspi = hspi;
    l3gd20_scale = scale;
    uint8_t who_am_i;

    if (l3gd20_hspi == NULL) {
        return L3GD20_ERROR;
    }

    /* check if sensor is L3GD20 */
    if (l3gd20_read_spi(L3GD20_REG_WHO_AM_I, &who_am_i) != L3GD20_OK) {
        return L3GD20_ERROR;
    }

    /* sensor connected is not L3GD20 */
    if (who_am_i != L3GD20_WHO_AM_I) {
        return L3GD20_ERROR;
    }

    /* enable L3GD20 Power bit */
    if (l3gd20_write_spi(L3GD20_REG_CTRL_REG1, 0xFF) != L3GD20_OK) {
        return L3GD20_ERROR;
    }

    /* set L3GD20 scale */
    if (l3gd20_scale == L3GD20_SCALE_250) {
        if (l3gd20_write_spi(L3GD20_REG_CTRL_REG4, 0x00) != L3GD20_OK) {
            return L3GD20_ERROR;
        }
    } else if (l3gd20_scale == L3GD20_SCALE_500) {
        if (l3gd20_write_spi(L3GD20_REG_CTRL_REG4, 0x10) != L3GD20_OK) {
            return L3GD20_ERROR;
        }
    } else if (l3gd20_scale == L3GD20_SCALE_2000) {
        if (l3gd20_write_spi(L3GD20_REG_CTRL_REG4, 0x20) != L3GD20_OK) {
            return L3GD20_ERROR;
        }
    }

    /* set high-pass filter settings */
    if (l3gd20_write_spi(L3GD20_REG_CTRL_REG2, 0x00) != L3GD20_OK) {
        return L3GD20_ERROR;
    }

    /* enable high-pass filter */
    if (l3gd20_write_spi(L3GD20_REG_CTRL_REG5, 0x10) != L3GD20_OK) {
        return L3GD20_ERROR;
    }

    return L3GD20_OK;
}

l3gd20_result_t l3gd20_read(l3gd20_data_t *data) {
    float temp, s;
    uint8_t byte_lo, byte_hi;

    /* read X axis */
    if (l3gd20_read_spi(L3GD20_REG_OUT_X_L, &byte_lo) != L3GD20_OK) {
        return L3GD20_ERROR;
    }

    if (l3gd20_read_spi(L3GD20_REG_OUT_X_H, &byte_hi) != L3GD20_OK) {
        return L3GD20_ERROR;
    }

    data->x = byte_hi << 8 | byte_lo;

    /* read Y axis */
    if (l3gd20_read_spi(L3GD20_REG_OUT_Y_L, &byte_lo) != L3GD20_OK) {
        return L3GD20_ERROR;
    }

    if (l3gd20_read_spi(L3GD20_REG_OUT_Y_H, &byte_hi) != L3GD20_OK) {
        return L3GD20_ERROR;
    }

    data->y = byte_hi << 8 | byte_lo;

    /* read Z axis */
    if (l3gd20_read_spi(L3GD20_REG_OUT_Z_L, &byte_lo) != L3GD20_OK) {
        return L3GD20_ERROR;
    }

    if (l3gd20_read_spi(L3GD20_REG_OUT_Z_H, &byte_hi) != L3GD20_OK) {
        return L3GD20_ERROR;
    }

    data->z = byte_hi << 8 | byte_lo;

    /* set sensitivity scale correction */
    if (l3gd20_scale == L3GD20_SCALE_250) {
        s = L3GD20_SENSITIVITY_250 * 0.001;
    } else if (l3gd20_scale == L3GD20_SCALE_500) {
        s = L3GD20_SENSITIVITY_500 * 0.001;
    } else {
        s = L3GD20_SENSITIVITY_2000 * 0.001;
    }

    /* convert results */
    temp = (float) data->x * s;
    data->x = (int16_t) temp;
    temp = (float) data->y * s;
    data->y = (int16_t) temp;
    temp = (float) data->z * s;
    data->z = (int16_t) temp;

    return L3GD20_OK;
}

l3gd20_result_t l3gd20_read_spi(uint8_t address, uint8_t *data) {
    uint8_t address_out = address | 0x80;

    L3GD20_CS_LOW;

    if (HAL_SPI_Transmit(l3gd20_hspi, &address_out, 1, 50) != HAL_OK) {
        return L3GD20_ERROR;
    }
    if (HAL_SPI_Receive(l3gd20_hspi, data, 1, 50) != HAL_OK) {
        return L3GD20_ERROR;
    }

    L3GD20_CS_HIGH;

    return L3GD20_OK;
}

l3gd20_result_t l3gd20_write_spi(uint8_t address, uint8_t data) {
    uint8_t buf[2] = { address, data };

    L3GD20_CS_LOW;

    if (HAL_SPI_Transmit(l3gd20_hspi, buf, 2, 100) != HAL_OK) {
        return L3GD20_ERROR;
    }

    L3GD20_CS_HIGH;

    return L3GD20_OK;
}

