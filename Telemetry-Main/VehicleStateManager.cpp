#include "VehicleStateManager.hpp"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <numeric>
#include "CANInterface.hpp"
#include "CANProtocol.hpp"
#include "mbed.h"

VehicleStateManager::VehicleStateManager(
    MbedCAN* mbedCAN,
    PinName steering_sensor,
    PinName brake_sensor_f,
    PinName brake_sensor_r
) : _mbedCAN(mbedCAN), _steering_sensor(steering_sensor), _brake_sensor_f(brake_sensor_r), _brake_sensor_r(brake_sensor_r)
{
    _vehicleState = {};
}

void VehicleStateManager::update() {
    // printf("Updating CAN\n");
    processCANMessage();
    readSensorValues();
}

void VehicleStateManager::readSensorValues() {
    _vehicleState.steering_sensor =  _steering_sensor.read() * 3.3f;

    _vehicleState.brake_sensor_f =_brake_sensor_f.read() * 3.3f;
    _vehicleState.brake_sensor_r =_brake_sensor_r.read() * 3.3f;
}

void VehicleStateManager::processCANMessage() {
    if (!_mbedCAN || !_mbedCAN->isReady()) { return; }
    CANMessage msg;
    while (_mbedCAN->read(msg)) {
        switch (msg.id) {
            // ACC Message
            case CAN_ID::ACC_STATUS: {
                const auto* data = reinterpret_cast<const ACC_STATUS_t*>(msg.data);
                _vehicleState.accStatus = *data;
                break;
            }
            case CAN_ID::ACC_SEG0_VOLTS:
            case CAN_ID::ACC_SEG1_VOLTS:
            case CAN_ID::ACC_SEG2_VOLTS:
            case CAN_ID::ACC_SEG3_VOLTS:
            case CAN_ID::ACC_SEG4_VOLTS: {
                int segment = -1;
                if (msg.id == CAN_ID::ACC_SEG0_VOLTS) segment = 0;
                else if (msg.id == CAN_ID::ACC_SEG1_VOLTS) segment = 1;
                else if (msg.id == CAN_ID::ACC_SEG2_VOLTS) segment = 2;
                else if (msg.id == CAN_ID::ACC_SEG3_VOLTS) segment = 3;
                else if (msg.id == CAN_ID::ACC_SEG4_VOLTS) segment = 4;
                if (segment != -1) {
                    const ACC_SEG_VOLTS_t* data = reinterpret_cast<const ACC_SEG_VOLTS_t*>(msg.data);
                    _vehicleState.accSegVolts[segment] = *data;
                }
                break;
            }
            case CAN_ID::ACC_POWER: {
                const ACC_POWER_t* data = reinterpret_cast<const ACC_POWER_t*>(msg.data);
                _vehicleState.accPower = *data;
                break;
            }
            case CAN_ID::ACC_SEG0_TEMPS:
            case CAN_ID::ACC_SEG1_TEMPS:
            case CAN_ID::ACC_SEG2_TEMPS:
            case CAN_ID::ACC_SEG3_TEMPS:
            case CAN_ID::ACC_SEG4_TEMPS: {
                int segment = -1;
                if (msg.id == CAN_ID::ACC_SEG0_TEMPS) segment = 0;
                else if (msg.id == CAN_ID::ACC_SEG1_TEMPS) segment = 1;
                else if (msg.id == CAN_ID::ACC_SEG2_TEMPS) segment = 2;
                else if (msg.id == CAN_ID::ACC_SEG3_TEMPS) segment = 3;
                else if (msg.id == CAN_ID::ACC_SEG4_TEMPS) segment = 4;
                if (segment != -1) {
                    const ACC_SEG_TEMPS_t* data = reinterpret_cast<const ACC_SEG_TEMPS_t*>(msg.data);
                    _vehicleState.accSegTemps[segment] = *data;
                }
                break;
            }
            // ETC Messages
            case CAN_ID::ETC_STATUS: {
                const ETC_STATUS_t* data = reinterpret_cast<const ETC_STATUS_t*>(msg.data);
                _vehicleState.etcStatus = *data;
                // printf("pedal travel: %d\n", _vehicleState.etcStatus.PEDAL_TRAVEL);
                break;
            }
            // PDB Messages
            case CAN_ID::PDB_POWER_A: {
                const PDB_POWER_A_t* data = reinterpret_cast<const PDB_POWER_A_t*>(msg.data);
                _vehicleState.pdbPowerA = *data;
                break;
            }
            case CAN_ID::PDB_POWER_B: {
                const PDB_POWER_B_t* data = reinterpret_cast<const PDB_POWER_B_t*>(msg.data);
                _vehicleState.pdbPowerB = *data;
                break;
            }
            // SME Messages
            case CAN_ID::SME_THROTTLE_DEMAND: {
                const SME_THROTTLE_DEMAND_t* data = reinterpret_cast<const SME_THROTTLE_DEMAND_t*>(msg.data);
                _vehicleState.smeThrottleDemand = *data;
                break;
            }
            case CAN_ID::SME_MAX_CURRENTS: {
                const SME_MAX_CURRENTS_t* data = reinterpret_cast<const SME_MAX_CURRENTS_t*>(msg.data);
                _vehicleState.smeMaxCurrents = *data;
                break;
            }
            case CAN_ID::SME_TRQSPD: {
                const SME_TRQSPD_t* data = reinterpret_cast<const SME_TRQSPD_t*>(msg.data);
                _vehicleState.smeTrqSpd = *data;
                break;
            }
            case CAN_ID::SME_TEMP: {
                const SME_TEMP_t* data = reinterpret_cast<const SME_TEMP_t*>(msg.data);
                _vehicleState.smeTemp = *data;
                break;
            }
            // TMAIN Messages
            case CAN_ID::TMAIN_DATA: {
                const TMAIN_DATA_t* data = reinterpret_cast<const TMAIN_DATA_t*>(msg.data);
                _vehicleState.tmainData = *data;
                break;
            }
            // TPERIPH Messages
            case CAN_ID::TPERIPH_FL_DATA:
            case CAN_ID::TPERIPH_FR_DATA:
            case CAN_ID::TPERIPH_BL_DATA:
            case CAN_ID::TPERIPH_BR_DATA: {
                int wheel_idx = -1;
                if (msg.id == CAN_ID::TPERIPH_FL_DATA) wheel_idx = 0;      // Front Left
                else if (msg.id == CAN_ID::TPERIPH_FR_DATA) wheel_idx = 1; // Front Right
                else if (msg.id == CAN_ID::TPERIPH_BL_DATA) wheel_idx = 2; // Back Left
                else if (msg.id == CAN_ID::TPERIPH_BR_DATA) wheel_idx = 3; // Back Right
                if (wheel_idx != -1) {
                    const TPERIPH_DATA_t* data = reinterpret_cast<const TPERIPH_DATA_t*>(msg.data);
                    _vehicleState.tperiphData[wheel_idx] = *data;
                }
                break;
            }
            case CAN_ID::TPERIPH_FL_TIRETEMP:
            case CAN_ID::TPERIPH_FR_TIRETEMP:
            case CAN_ID::TPERIPH_BL_TIRETEMP:
            case CAN_ID::TPERIPH_BR_TIRETEMP: {
                int tire_idx = -1;
                if (msg.id == CAN_ID::TPERIPH_FL_TIRETEMP) tire_idx = 0;      // Front Left
                else if (msg.id == CAN_ID::TPERIPH_FR_TIRETEMP) tire_idx = 1; // Front Right
                else if (msg.id == CAN_ID::TPERIPH_BL_TIRETEMP) tire_idx = 2; // Back Left
                else if (msg.id == CAN_ID::TPERIPH_BR_TIRETEMP) tire_idx = 3; // Back Right
                if (tire_idx != -1) {
                    const TPERIPH_TIRETEMP_t* data = reinterpret_cast<const TPERIPH_TIRETEMP_t*>(msg.data);
                    _vehicleState.tperiphTireTemp[tire_idx] = *data;
                }
                break;
            }
            // VDM Messages
            //
            // 
            // (TODO: REVERSE ORDER)
            // 
            //
            case CAN_ID::VDM_GPS_LAT_LONG: {
                const VDM_GPS_LAT_LONG_t* data = reinterpret_cast<const VDM_GPS_LAT_LONG_t*>(msg.data);
                _vehicleState.vdmGpsLatLong = *data;
                break;
            }
            case CAN_ID::VDM_GPS_DATA: {
                const VDM_GPS_DATA_t* data = reinterpret_cast<const VDM_GPS_DATA_t*>(msg.data);
                _vehicleState.vdmGpsData = *data;
                break;
            }
            case CAN_ID::VDM_DATE_TIME: {
                const VDM_DATE_TIME_t* data = reinterpret_cast<const VDM_DATE_TIME_t*>(msg.data);
                _vehicleState.vdmDateTime = *data;
                break;
            }
            case CAN_ID::VDM_ACCELERATION: {
                const VDM_ACCELERATION_t* data = reinterpret_cast<const VDM_ACCELERATION_t*>(msg.data);
                _vehicleState.vdmAcceleration = *data;
                break;
            }
            case CAN_ID::VDM_YAW_RATE: {
                const VDM_YAW_RATE_t* data = reinterpret_cast<const VDM_YAW_RATE_t*>(msg.data);
                _vehicleState.vdmYawRate = *data;
                break;
            }
            default:
                // unhandled CAN ID
                break;
        }
    }
}

VehicleState VehicleStateManager::getState() const {
    return _vehicleState;
}
