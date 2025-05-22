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

// Returns Capacity In mAh
int16_t convertLowVoltage(float voltage);

// filteredTsCurrent
int8_t currStateOfCharge(int8_t previousStateOfCharge, int changeInTime, uint16_t currentNow, uint16_t currentPrev);

//Linear Interpolation Function
static int16_t linearInterpolateAh(SOCConversion low, SOCConversion high, float voltage);