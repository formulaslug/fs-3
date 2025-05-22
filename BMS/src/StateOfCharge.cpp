/*
Variables:
SoC previous, change in time, current now and current previously
 */
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>

#include <array>
#include <cstdint>

struct SOCConversion {
    float voltage;
    float capacity;
};

static constexpr size_t socLookupTableSize = 15;
// Pairs of Measured Voltages(V) & Corresponding Capacities Of Battery(Ah). V -> Ah // TODO: CHANGE INTO mV and mAh -> make everything into ints (faster for processor)
static constexpr std::array<SOCConversion, socLookupTableSize>
socLookupTable {{
    {2800,2500}, {2900,2480}, {3000,2400}, {3100,2350}, {3200,2250},
    {3300,2170}, {3400,2020}, {3500,1800}, {3600,1600}, {3700,1270},
    {3800,910}, {3900,710}, {4000,400}, {4100,180}, {4200,0}
}};

static int16_t linearInterpolateAh(SOCConversion low, SOCConversion high, float voltage);

// Returns Capacity In mAh
int16_t convertLowVoltage(float voltage) {
    for(unsigned int i = 0; i < socLookupTableSize; i++) {
        if(voltage > socLookupTable[i].voltage) {
            if(i != socLookupTableSize - 1) {
                return linearInterpolateAh(socLookupTable[i], socLookupTable[i-1], voltage);
            } else {
                return static_cast<int16_t>(socLookupTable[i].capacity * 1000);
            }
        }
    }
    // Fallback Value
    return {};
}

static int16_t linearInterpolateAh(SOCConversion low, SOCConversion high, float voltage) {

    // If something went wrong with convertLowVoltage
    // if(voltage < low.capacity || voltage > high.capacity) return 100;

    return ((low.capacity + ( (voltage - low.voltage) * (high.capacity - low.capacity) / (high.voltage - low.voltage)) ) * 1000) ;
}

// calculates the depthofdischarge with a riemann sum; must be added to the current amt of discharge for total sum
int8_t currStateOfCharge(int8_t previousStateOfCharge, int deltaTime, uint16_t currentNow, uint16_t currentPrev) {
  int8_t currentStateOfCharge = previousStateOfCharge - (deltaTime * (currentNow + currentPrev) / 200);

  return currentStateOfCharge;
 }