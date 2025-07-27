#ifndef VEHICLE_STATE_MANAGER_HPP
#define VEHICLE_STATE_MANAGER_HPP

#include <cstdint>
#include <string>
#include "CANProtocol.hpp"
#include "CANInterface.hpp"
#include "mbed.h"

#if defined(__linux__)
#include <byteswap.h>
#define bswap_16(x) bswap_16(x)
#define bswap_32(x) bswap_32(x)
#define bswap_64(x) bswap_64(x)
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define bswap_16(x) OSSwapInt16(x)
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)
#endif

struct VehicleState {
    // ACC
    ACC_STATUS_t accStatus;
    ACC_SEG_VOLTS_t accSegVolts[5];
    ACC_SEG_TEMPS_t accSegTemps[5];
    ACC_POWER_t accPower;
    // ETC
    ETC_STATUS_t etcStatus;
    // PDB
    PDB_POWER_A_t pdbPowerA;
    PDB_POWER_B_t pdbPowerB;
    // SME
    SME_THROTTLE_DEMAND_t smeThrottleDemand;
    SME_TRQSPD_t smeTrqSpd;
    SME_TEMP_t smeTemp;
    SME_MAX_CURRENTS_t smeMaxCurrents;
    // TMAIN
    TMAIN_DATA_t tmainData;
    // TPERIPH (per wheel)
    TPERIPH_DATA_t tperiphData[4];         // 0:FL, 1:FR, 2:BL, 3:BR
    TPERIPH_TIRETEMP_t tperiphTireTemp[4]; // 0:FL, 1:FR, 2:BL, 3:BR
    // VDM
    VDM_GPS_LAT_LONG_t vdmGpsLatLong;
    VDM_GPS_DATA_t vdmGpsData;
    VDM_DATE_TIME_t vdmDateTime;
    VDM_ACCELERATION_t vdmAcceleration;
    VDM_YAW_RATE_t vdmYawRate;
};

class VehicleStateManager {

public:
    explicit VehicleStateManager(ICAN* ican);
    
    VehicleState getState() const;
    void update();
    void startLapTimer();
    const char* getLapTime() const;

private:
    ICAN* _ican;

    VehicleState _vehicleState;
    char _lapTime[16];
    Timer _lapTimer;

    void processCANMessage();
    void updateLapTime();
};

#endif // VEHICLE_STATE_MANAGER_HPP 