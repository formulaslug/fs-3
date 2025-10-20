#ifndef VEHICLE_STATE_MANAGER_HPP
#define VEHICLE_STATE_MANAGER_HPP

#include <cstdint>
#include <string>
#include "CANProtocol.hpp"
#include "CANInterface.hpp"
#include "mbed.h"

struct VehicleState {
    // ---- FROM CAN BUS ----
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

    // ---- FROM NUCLEO PINS ----
    float steering_sensor;
    float brake_sensor_f;
    float brake_sensor_r;
};

class VehicleStateManager {

public:
    explicit VehicleStateManager(
        MbedCAN* mbedCAN,
        PinName steering_sensor,
        PinName brake_sensor_f,
        PinName brake_sensor_r
    );
    
    VehicleState getState() const;
    void update();
    /**
     * Resets all fields relating to lap counting to match current
     * VehicleState. Assumes VehicleState was recently updated.
     */
    void resetLapCounter();
    /**
     * Returns true if a lap was detected as completed
     */
    bool lapCompleted();
    void startLapTimer();
    const char* getLapTime() const;

private:
    MbedCAN* _mbedCAN;
    AnalogIn _steering_sensor;
    AnalogIn _brake_sensor_f;
    AnalogIn _brake_sensor_r;

    VehicleState _vehicleState;

    char _lapTime[16];
    Timer _lapTimer;
    //  LowPowerTimer _lapTimer;

    void processCANMessage();
    void updateLapTime();
    void readSensorValues();
};

#endif // VEHICLE_STATE_MANAGER_HPP 
