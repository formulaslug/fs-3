#include "stm32f3xx_lsm303dlhc.h"

/* private variables */
static I2C_HandleTypeDef *lsm303dlhc_i2c = NULL;
static lsm303dlhc_mag_gain_t lsm303dlhc_mag_magin = LSM303DLHC_MAGGAIN_1_3;
static bool lsm303dlhc_mag_auto_range = false;
static float lsm303dlhc_acc_mg_lsb = 0.001f;   // 1, 2, 4 or 12 mg per lsb
static float lsm303dlhc_mag_gauss_lsb_xy = 1100.0f;  // Varies with gain
static float lsm303dlhc_mag_gauss_lsb_z = 980.0f;   // Varies with gain

/* private functions */
static lsm303dlhc_result_t lsm303dlhc_read_i2c(uint8_t address, uint8_t reg, uint8_t *data);
static lsm303dlhc_result_t lsm303dlhc_write_i2c(uint8_t address, uint8_t reg, uint8_t data);

lsm303dlhc_result_t lsm303dlhc_init_acc(I2C_HandleTypeDef *i2c, const lsm303dlhc_acc_init_t *init) {
    uint8_t reg1_a;
    lsm303dlhc_i2c = i2c;

    if (lsm303dlhc_i2c == NULL || init == NULL) {
        return LSM303DLHC_ERROR;
    }

    /* set control registers */
    if (lsm303dlhc_write_i2c(LSM303DLHC_ADDR_ACC, LSM303DLHC_REG_ACC_CTRL_REG1_A, init->ctrl_reg1_a) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }

    if (lsm303dlhc_write_i2c(LSM303DLHC_ADDR_ACC, LSM303DLHC_REG_ACC_CTRL_REG2_A, init->ctrl_reg2_a) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }

    if (lsm303dlhc_write_i2c(LSM303DLHC_ADDR_ACC, LSM303DLHC_REG_ACC_CTRL_REG3_A, init->ctrl_reg3_a) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }

    if (lsm303dlhc_set_acc_scale(init->ctrl_reg4_a) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }

    if (lsm303dlhc_write_i2c(LSM303DLHC_ADDR_ACC, LSM303DLHC_REG_ACC_CTRL_REG5_A, init->ctrl_reg5_a) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }

    if (lsm303dlhc_write_i2c(LSM303DLHC_ADDR_ACC, LSM303DLHC_REG_ACC_CTRL_REG6_A, init->ctrl_reg6_a) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }

    /* LSM303DLHC has no WHOAMI register so read CTRL_REG1_A back to check if we are connected or not */
    if (lsm303dlhc_read_i2c(LSM303DLHC_ADDR_ACC, LSM303DLHC_REG_ACC_CTRL_REG1_A, &reg1_a) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }
    if (reg1_a != init->ctrl_reg1_a) {
        return LSM303DLHC_ERROR;
    }

    return LSM303DLHC_OK;
}

lsm303dlhc_result_t lsm303dlhc_set_acc_scale(uint8_t ctrl_reg4_a) {
    if (lsm303dlhc_write_i2c(LSM303DLHC_ADDR_ACC, LSM303DLHC_REG_ACC_CTRL_REG4_A, ctrl_reg4_a) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }

    if ((ctrl_reg4_a & 0b110000) == LSM303DLHC_ACR4A_FS10_1MG) {
        lsm303dlhc_acc_mg_lsb = 0.001f;
    } else if ((ctrl_reg4_a & 0b110000) == LSM303DLHC_ACR4A_FS10_2MG) {
        lsm303dlhc_acc_mg_lsb = 0.002f;
    } else if ((ctrl_reg4_a & 0b110000) == LSM303DLHC_ACR4A_FS10_4MG) {
        lsm303dlhc_acc_mg_lsb = 0.004f;
    } else if ((ctrl_reg4_a & 0b110000) == LSM303DLHC_ACR4A_FS10_12MG) {
        lsm303dlhc_acc_mg_lsb = 0.012f;
    }

    return LSM303DLHC_OK;
}

lsm303dlhc_result_t lsm303dlhc_read_acc_raw(lsm303dlhc_data_raw_t *data) {
    uint8_t reg = LSM303DLHC_REG_ACC_OUT_X_L_A | 0x80;
    uint8_t buf[6] = { 0 };

    if (HAL_I2C_Master_Transmit(lsm303dlhc_i2c, LSM303DLHC_ADDR_ACC, &reg, 1, 1000) != HAL_OK) {
        return LSM303DLHC_ERROR;
    }

    if (HAL_I2C_Master_Receive(lsm303dlhc_i2c, LSM303DLHC_ADDR_ACC, buf, 6, 1000) != HAL_OK) {
        return LSM303DLHC_ERROR;
    }

    /* raw data (low byte first) */
    data->x = (int16_t) (buf[LSM303DLHC_ACC_XLO] | (buf[LSM303DLHC_ACC_XHI] << 8)) >> 4;
    data->y = (int16_t) (buf[LSM303DLHC_ACC_YLO] | (buf[LSM303DLHC_ACC_YHI] << 8)) >> 4;
    data->z = (int16_t) (buf[LSM303DLHC_ACC_ZLO] | (buf[LSM303DLHC_ACC_ZHI] << 8)) >> 4;

    return LSM303DLHC_OK;
}

void lsm303dlhc_convert_acc(lsm303dlhc_data_t *conv, const lsm303dlhc_data_raw_t *raw) {
    conv->x = (float) raw->x * lsm303dlhc_acc_mg_lsb * LSM303DLHC_ACC_SENSORS_GRAVITY_STANDARD;
    conv->y = (float) raw->y * lsm303dlhc_acc_mg_lsb * LSM303DLHC_ACC_SENSORS_GRAVITY_STANDARD;
    conv->z = (float) raw->z * lsm303dlhc_acc_mg_lsb * LSM303DLHC_ACC_SENSORS_GRAVITY_STANDARD;
}

lsm303dlhc_result_t lsm303dlhc_init_mag(I2C_HandleTypeDef *i2c, const lsm303dlhc_mag_init_t *init) {
    uint8_t cra_reg_m;
    lsm303dlhc_i2c = i2c;
    lsm303dlhc_mag_auto_range = init->auto_range;

    if (lsm303dlhc_i2c == NULL || init == NULL) {
        return LSM303DLHC_ERROR;
    }

    if (lsm303dlhc_write_i2c(LSM303DLHC_ADDR_MAG, LSM303DLHC_REG_MAG_MR_REG_M, (uint8_t) init->op) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }

    if (lsm303dlhc_set_mag_rate(init->rate) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }

    if (lsm303dlhc_set_mag_gain(init->gain) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }

    /* LSM303DLHC has no WHOAMI register so read CRA_REG_M to check the set value */
    if (lsm303dlhc_read_i2c(LSM303DLHC_ADDR_MAG, LSM303DLHC_REG_MAG_CRA_REG_M, &cra_reg_m) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }

    if (((cra_reg_m >> 2) & 0x07) != (uint8_t) init->rate) {
        return LSM303DLHC_ERROR;
    }

    return LSM303DLHC_OK;
}

lsm303dlhc_result_t lsm303dlhc_set_mag_rate(lsm303dlhc_mag_rate_t rate) {
    uint8_t reg_m = ((uint8_t) rate & 0x07) << 2;

    if (lsm303dlhc_write_i2c(LSM303DLHC_ADDR_MAG, LSM303DLHC_REG_MAG_CRA_REG_M, reg_m) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    } else {
        return LSM303DLHC_OK;
    }
}

lsm303dlhc_result_t lsm303dlhc_set_mag_gain(lsm303dlhc_mag_gain_t gain) {
    if (lsm303dlhc_write_i2c(LSM303DLHC_ADDR_MAG, LSM303DLHC_REG_MAG_CRB_REG_M, (uint8_t) gain) != LSM303DLHC_OK) {
        return LSM303DLHC_ERROR;
    }

    lsm303dlhc_mag_magin = gain;

    switch (lsm303dlhc_mag_magin) {
    case LSM303DLHC_MAGGAIN_1_3:
        lsm303dlhc_mag_gauss_lsb_xy = 1100;
        lsm303dlhc_mag_gauss_lsb_z = 980;
        break;
    case LSM303DLHC_MAGGAIN_1_9:
        lsm303dlhc_mag_gauss_lsb_xy = 855;
        lsm303dlhc_mag_gauss_lsb_z = 760;
        break;
    case LSM303DLHC_MAGGAIN_2_5:
        lsm303dlhc_mag_gauss_lsb_xy = 670;
        lsm303dlhc_mag_gauss_lsb_z = 600;
        break;
    case LSM303DLHC_MAGGAIN_4_0:
        lsm303dlhc_mag_gauss_lsb_xy = 450;
        lsm303dlhc_mag_gauss_lsb_z = 400;
        break;
    case LSM303DLHC_MAGGAIN_4_7:
        lsm303dlhc_mag_gauss_lsb_xy = 400;
        lsm303dlhc_mag_gauss_lsb_z = 355;
        break;
    case LSM303DLHC_MAGGAIN_5_6:
        lsm303dlhc_mag_gauss_lsb_xy = 330;
        lsm303dlhc_mag_gauss_lsb_z = 295;
        break;
    case LSM303DLHC_MAGGAIN_8_1:
        lsm303dlhc_mag_gauss_lsb_xy = 230;
        lsm303dlhc_mag_gauss_lsb_z = 205;
        break;
    }

    return LSM303DLHC_OK;
}

lsm303dlhc_result_t lsm303dlhc_read_mag_raw(lsm303dlhc_data_raw_t *data) {
    bool reading_valid = false;
    uint8_t reg_mg = 0;
    uint8_t reg_mag_out = LSM303DLHC_REG_MAG_OUT_X_H_M;
    uint8_t buf[6] = { 0 };

    while (reading_valid == false) {
        if (lsm303dlhc_read_i2c(LSM303DLHC_ADDR_MAG, LSM303DLHC_REG_MAG_SR_REG_Mg, &reg_mg) != LSM303DLHC_OK) {
            return LSM303DLHC_ERROR;
        }

        if (!(reg_mg & 0x1)) {
            return LSM303DLHC_ERROR;
        }

        if (HAL_I2C_Master_Transmit(lsm303dlhc_i2c, LSM303DLHC_ADDR_MAG, &reg_mag_out, 1, 1000) != HAL_OK) {
            return LSM303DLHC_ERROR;
        }

        if (HAL_I2C_Master_Receive(lsm303dlhc_i2c, LSM303DLHC_ADDR_MAG, buf, 6, 1000) != HAL_OK) {
            return LSM303DLHC_ERROR;
        }

        /* raw data (low byte first) */
        data->x = (int16_t) (buf[LSM303DLHC_MAG_XLO] | (buf[LSM303DLHC_MAG_XHI] << 8));
        data->y = (int16_t) (buf[LSM303DLHC_MAG_YLO] | (buf[LSM303DLHC_MAG_YHI] << 8));
        data->z = (int16_t) (buf[LSM303DLHC_MAG_ZLO] | (buf[LSM303DLHC_MAG_ZHI] << 8));

        /* make sure the sensor isn't saturating if auto-ranging is enabled */
        if (lsm303dlhc_mag_auto_range == false) {
            reading_valid = true;
        } else {
            /* check if the sensor is saturating or not */
            if ((data->x >= 2040) | (data->x <= -2040) | (data->y >= 2040) | (data->y <= -2040) | (data->z >= 2040) | (data->z <= -2040)) {
                /* saturating .... increase the range if we can */
                switch (lsm303dlhc_mag_magin) {
                case LSM303DLHC_MAGGAIN_5_6:
                    if (lsm303dlhc_set_mag_gain(LSM303DLHC_MAGGAIN_8_1) != LSM303DLHC_OK) {
                        return LSM303DLHC_ERROR;
                    } else {
                        reading_valid = false;
                    }
                    break;

                case LSM303DLHC_MAGGAIN_4_7:
                    if (lsm303dlhc_set_mag_gain(LSM303DLHC_MAGGAIN_5_6) != LSM303DLHC_OK) {
                        return LSM303DLHC_ERROR;
                    } else {
                        reading_valid = false;
                    }
                    break;

                case LSM303DLHC_MAGGAIN_4_0:
                    if (lsm303dlhc_set_mag_gain(LSM303DLHC_MAGGAIN_4_7) != LSM303DLHC_OK) {
                        return LSM303DLHC_ERROR;
                    } else {
                        reading_valid = false;
                    }
                    break;

                case LSM303DLHC_MAGGAIN_2_5:
                    if (lsm303dlhc_set_mag_gain(LSM303DLHC_MAGGAIN_4_0) != LSM303DLHC_OK) {
                        return LSM303DLHC_ERROR;
                    } else {
                        reading_valid = false;
                    }
                    break;

                case LSM303DLHC_MAGGAIN_1_9:
                    if (lsm303dlhc_set_mag_gain(LSM303DLHC_MAGGAIN_2_5) != LSM303DLHC_OK) {
                        return LSM303DLHC_ERROR;
                    } else {
                        reading_valid = false;
                    }
                    break;

                case LSM303DLHC_MAGGAIN_1_3:
                    if (lsm303dlhc_set_mag_gain(LSM303DLHC_MAGGAIN_1_9) != LSM303DLHC_OK) {
                        return LSM303DLHC_ERROR;
                    } else {
                        reading_valid = false;
                    }
                    break;

                    /* cannot change the range */
                default:
                    reading_valid = true;
                    break;
                }
            } else {
                /* all values are withing range */
                reading_valid = true;
            }
        }
    }

    return LSM303DLHC_OK;
}

void lsm303dlhc_convert_mag(lsm303dlhc_data_t *conv, const lsm303dlhc_data_raw_t *raw) {
    conv->x = ((float) raw->x / lsm303dlhc_mag_gauss_lsb_xy) * LSM303DLHC_MAG_SENSORS_GAUSS_TO_MICROTESLA;
    conv->y = ((float) raw->y / lsm303dlhc_mag_gauss_lsb_xy) * LSM303DLHC_MAG_SENSORS_GAUSS_TO_MICROTESLA;
    conv->z = ((float) raw->z / lsm303dlhc_mag_gauss_lsb_z) * LSM303DLHC_MAG_SENSORS_GAUSS_TO_MICROTESLA;
}

/* private functions */
static lsm303dlhc_result_t lsm303dlhc_read_i2c(uint8_t address, uint8_t reg, uint8_t *data) {
    if (HAL_I2C_Master_Transmit(lsm303dlhc_i2c, address, &reg, 1, 1000) != HAL_OK) {
        return LSM303DLHC_ERROR;
    }

    if (HAL_I2C_Master_Receive(lsm303dlhc_i2c, address, data, 1, 1000) != HAL_OK) {
        return LSM303DLHC_ERROR;
    }

    return LSM303DLHC_OK;
}

static lsm303dlhc_result_t lsm303dlhc_write_i2c(uint8_t address, uint8_t reg, uint8_t data) {
    uint8_t buf[2] = { reg, data };

    if (HAL_I2C_Master_Transmit(lsm303dlhc_i2c, address, buf, 2, 1000) != HAL_OK) {
        return LSM303DLHC_ERROR;
    } else {
        return LSM303DLHC_OK;
    }
}
