#include "mcc_generated_files/examples/twi0_master_example.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define N_PIXEL 8
double d6t_8lh_ptat;
double d6t_8lh_pix_data[N_PIXEL];

uint8_t d6t_8lh_calc_crc(uint8_t data);

/* D6T PEC(Packet Error Check) calculation.
 * calculate the data sequence,
 * from an I2C Read client address (8bit) to thermal data end.
 */
bool d6t_8lh_checkPEC(uint8_t buf[], int n);

/* convert a 16bit data from the byte stream. */
int16_t d6t_8lh_conv8us_s16_le(uint8_t *buf, int n);

/* setup:
 * 1. Initialize
       - initialize a Serial port for output.
           - initialize I2C.
           - Send inisilize setting to D6T.
 */
void d6t_8lh_setup();

/* loop - Thermal sensor
 * 2. read data.
 */
double d6t_8lh_loop();
