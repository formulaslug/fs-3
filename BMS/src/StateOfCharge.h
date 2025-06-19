/*
Variables:
SoC previous, change in time, current now and current previously
 */
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>

#include <array>
#include <cstdint>

struct SOCConversion;

// Returns Capacity In mAh from pack voltage
uint16_t convertLowVoltage(uint32_t voltage);

//Linear Interpolation Function
static uint16_t linearInterpolateAh(SOCConversion low, SOCConversion high, uint32_t voltage);

static uint16_t soc_energy(uint16_t voltage, int32_t capacity);