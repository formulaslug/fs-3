/*
Variables:
SoC previous, change in time, current now and current previously
 */
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>

#include <array>
#include <cstdint>

#include "StateOfCharge.h"
#include "BmsConfig.h"

struct SOCConversion {
    float voltage;
    float capacity;
};

static constexpr size_t socLookupTableSize = 15;
// Pairs of Measured Voltages(mV) & Corresponding Capacities Of Battery(mAh). mV -> mAh
static constexpr std::array<SOCConversion, socLookupTableSize>
socLookupTable {{
    {2800,50000}, {2900,49600}, {3000,48000}, {3100,47000}, {3200,45000},
    {3300,43400}, {3400,40400}, {3500,36000}, {3600,32000}, {3700,25400},
    {3800,18200}, {3900,14200}, {4000,8000}, {4100,3600}, {4200,0}
}};

// Returns Capacity In mAh
uint16_t convertLowVoltage(uint32_t voltage) {
    uint32_t cell_voltage = voltage / (BMS_BANK_COUNT * BMS_BANK_CELL_COUNT);
    // printf("cell voltage: %d\n", cell_voltage);
    for(unsigned int i = 0; i < socLookupTableSize - 1; i++) {
        if(cell_voltage < socLookupTable[i].voltage) {
            if (i == 0)
            {
                return CELL_CAPACITY_RATED;
            }
            return linearInterpolateAh(socLookupTable[i-1], socLookupTable[i], cell_voltage);
        }
    }
    // Fallback Value
    return 0;
}

static uint16_t linearInterpolateAh(SOCConversion low, SOCConversion high, uint32_t voltage) {

    // If something went wrong with convertLowVoltage
    // if(voltage < low.capacity || voltage > high.capacity) return 100;

    return ((low.capacity + ( (voltage - low.voltage) * (high.capacity - low.capacity) / (high.voltage - low.voltage)) )) ;
}
