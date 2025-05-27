#include "mcc_generated_files/examples/twi0_master_example.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define D6T_ADDR 0x0A // for I2C 7bit address
#define D6T_CMD 0x4C  // for D6T-44L-06/06H, D6T-8L-09/09H, for D6T-1A-01/02

#define N_ROW 8
#define N_PIXEL 8
#define N_READ ((N_PIXEL + 1) * 2 + 1)

uint8_t rbuf[N_READ];
double d6t_8lh_ptat;
double d6t_8lh_pix_data[N_PIXEL];

uint8_t calc_crc(uint8_t data) {
    int index;
    uint8_t temp;
    for (index = 0; index < 8; index++) {
        temp = data;
        data <<= 1;
        if (temp & 0x80) {
            data ^= 0x07;
        }
    }
    return data;
}

/* D6T PEC(Packet Error Check) calculation.
 * calculate the data sequence,
 * from an I2C Read client address (8bit) to thermal data end.
 */
bool D6T_checkPEC(uint8_t buf[], int n) {
    int i;
    uint8_t crc = calc_crc((D6T_ADDR << 1) | 1); // I2C Read address (8bit)
    for (i = 0; i < n; i++) {
        crc = calc_crc(buf[i] ^ crc);
    }
    bool ret = crc != buf[n];
    // Attiny doesn't have easy serial debug :/
    // if (ret) {
    //     Serial.print("PEC check failed:");
    //     Serial.print(crc, HEX);
    //     Serial.print("(cal) vs ");
    //     Serial.print(buf[n], HEX);
    //     Serial.println("(get)");
    // }
    return ret;
}

/* convert a 16bit data from the byte stream. */
int16_t conv8us_s16_le(uint8_t *buf, int n) {
    uint16_t ret;
    ret = (uint16_t)buf[n];
    ret += ((uint16_t)buf[n + 1]) << 8;
    return (int16_t)ret; // and convert negative.
}

/* setup:
 * 1. Initialize
       - initialize a Serial port for output.
           - initialize I2C.
           - Send inisilize setting to D6T.
 */
void d6t_8lh_setup() {
    char w1[4] = {0x02, 0x00, 0x01, 0xEE};
    char w2[4] = {0x05, 0x90, 0x3A, 0xB8};
    char w3[4] = {0x03, 0x00, 0x03, 0x8B};
    char w4[4] = {0x03, 0x00, 0x07, 0x97};
    char w5[4] = {0x02, 0x00, 0x00, 0xE9};

    I2C0_example_writeNBytes(D6T_ADDR, w1, sizeof(w1));
    I2C0_example_writeNBytes(D6T_ADDR, w2, sizeof(w2));
    I2C0_example_writeNBytes(D6T_ADDR, w3, sizeof(w3));
    I2C0_example_writeNBytes(D6T_ADDR, w4, sizeof(w4));
    I2C0_example_writeNBytes(D6T_ADDR, w5, sizeof(w5));
}

/* loop - Thermal sensor
 * 2. read data.
 */
void d6t_8lh_loop() {
    int i = 0;
    int16_t itemp = 0;

    // Read data via I2C
    // I2C buffer of "Arduino MKR" is 256 buffer. (It is enough)
    memset(rbuf, 0, N_READ);
    // Wire.beginTransmission(D6T_ADDR);  // I2C slave address
    // Wire.write(D6T_CMD);               // D6T register
    // Wire.endTransmission();
    // Wire.requestFrom(D6T_ADDR, N_READ);
    // while (Wire.available()) {
    //     rbuf[i++] = Wire.read();
    // }

    I2C0_example_readDataBlock(D6T_ADDR, D6T_CMD, rbuf, N_READ);

    D6T_checkPEC(rbuf, N_READ - 1);

    // Convert to temperature data (degC)
    d6t_8lh_ptat = (double)conv8us_s16_le(rbuf, 0) / 10.0;

    for (i = 0; i < N_PIXEL; i++) {
        itemp = conv8us_s16_le(rbuf, 2 + 2 * i);
        d6t_8lh_pix_data[i] = (double)itemp / 5.0;
    }

    // Attiny doesn't have easy serial debug :/
    //    //Output results
    //    Serial.print("PTAT:");
    //    Serial.print(ptat, 1);
    //    Serial.print(" [degC], Temperature: ");
    //    for (i = 0; i < N_PIXEL; i++) {
    // Serial.print(pix_data[i], 1);
    //     Serial.print(", ");
    //    }
    //    Serial.println(" [degC]");
}
