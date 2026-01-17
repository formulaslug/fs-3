#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include "Ticker.h"
#include "VehicleStateManager.hpp"
#include "dash/layouts.h"
#include "data_logger.hpp"
#include "fsdaq/encoder_generated.hpp"
#include "radio.hpp"
#include <stdbool.h>
#include <string>

constexpr bool ENABLE_RADIO = false;
constexpr bool ENABLE_SD = false;
constexpr bool ENABLE_DASH = true;

constexpr chrono::duration SD_UPDATE_HZ = 10ms;
constexpr chrono::duration DASH_UPDATE_HZ = 20ms;
constexpr chrono::duration RADIO_UPDATE_HZ = 100ms;

// Everything pertaining to XBee radio (TODO: clean)
DigitalIn xbee_spi_attn{PA_9};
DigitalOut xbee_spi_cs{PC_8};
SPI xbee_spi{PA_7, PA_6, PA_5};
XBeeRadio radio{xbee_spi, xbee_spi_cs, xbee_spi_attn};

auto mbed_can = CAN(PB_8, PB_9, 500000);
auto can = MbedCAN(mbed_can);
auto vsm = VehicleStateManager(&can, PC_5, PC_1, PC_0);
// auto vsm = VehicleStateManager(&can, PC_5, PA_0, PA_1);

// This still causes static initialization order fiasco, since DataLogger
// has a non-trivial constructor (which is called before main())!!!
// TODO: replace internal memeber variables (SDBlockDevice, etc) with deffered
// static initialization (eg. via optional.emplace(), singleton, COFU)
fsdaq::DataLogger data_logger{};
// TODO: Make DataBatch and DataRow templated over row count, and make a
// separate one for radio (with smaller row count)
fsdaq::DataRow current_row{};

Layouts eve(PC_12, PC_11, PC_10, PD_2, PB_7, PC_13, EvePresets::CFA800480E3);

EventQueue queue{EVENTS_QUEUE_SIZE};

void error_quit(std::string msg) {
    printf("%s\n", msg.c_str());
    while (1) {};
}

int n = 0;
void update_dash() {
    const VehicleState vsm_state = vsm.getState();

    // find max cell temp
    uint8_t max_temp = 0;
    for (auto [TEMPS_CELL0, TEMPS_CELL1, TEMPS_CELL2, TEMPS_CELL3, TEMPS_CELL4, TEMPS_CELL5] : vsm_state.accSegTemps) 
    {
       max_temp = max(TEMPS_CELL0, max(TEMPS_CELL1, max(TEMPS_CELL2, max(TEMPS_CELL3, max(TEMPS_CELL4, TEMPS_CELL5)))));
    }
    
    eve.drawMainDisplay(vsm_state.accStatus.SHUTDOWN_STATE, 
	vsm_state.smeTemp.FAULT_LEVEL, 
	vsm_state.etcStatus.RTD, 
	vsm_state.accStatus.PRECHARGE_DONE, 
	true /*fans*/, 
	vsm_state.accPower.PACK_VOLTAGE, 
	max_temp, 
	vsm_state.accPower.SOC, 
	n,
	vsm_state.vdmGpsData.SPEED, 
	vsm.getLapTime(), 
	vsm_state.accStatus.GLV_VOLTAGE, 
	vsm_state.smeTemp.MOTOR_TEMP, 
	vsm_state.smeTemp.CONTROLLER_TEMP,
	vsm_state.smeTemp.DC_BUS_V); 
	//eve.debugCellVolts(vsm_state.accSegVolts);
    //eve.debugCellTemps(vsm_state.accSegTemps);
}

int main() {
    printf("Hello world\n");

    // Initializing Dash
    if (ENABLE_DASH) {
        eve.init(EvePresets::CFA800480E3);
        update_dash();
    }

    // Initializing Data Logging
    if (ENABLE_SD) {
        data_logger.init_logging();
    }


    if (ENABLE_RADIO) {
        queue.call_every(RADIO_UPDATE_HZ, [&](){ radio.transmit((uint8_t*)&current_row, sizeof(current_row)); });
    }

    if (ENABLE_SD) {
        // TODO::::
        queue.call_every(SD_UPDATE_HZ, [&]() { data_logger.append_row(current_row); });

        // mbed_can.attach(
        //     []() { queue.call([]() { data_writer.append_row(current_row); }); },
        //     interface::can::RxIrq
        // );
    }
    if (ENABLE_DASH) {
        queue.call_every(DASH_UPDATE_HZ, &update_dash);
    }


    if (true){
        queue.call_every(100ms, []() {
            const VehicleState state = vsm.getState();
            const uint8_t tmain_data[] = {
                static_cast<uint8_t>(state.brake_sensor_f),
                static_cast<uint8_t>((state.brake_sensor_f & 0xFF00) >> 8),
                static_cast<uint8_t>(state.brake_sensor_r),
                static_cast<uint8_t>((state.brake_sensor_r & 0xFF00) >> 8),
                static_cast<uint8_t>(state.steering_sensor),
                static_cast<uint8_t>((state.steering_sensor & 0xFF00) >> 8),
            };
            can.write(CANMessage{0x1A0, tmain_data, 6});
        });
    }

    // Timer t;
    // t.start();
    queue.call_every(1ms, [/*&t*/]() {
        // printf("%fms\n", t.elapsed_time().count() / 1000.0f);
        // t.reset();
        vsm.update();
        const VehicleState state = vsm.getState();

        // SME
        current_row.SME_THROTL_TorqueDemand = state.smeThrottleDemand.TORQUE_DEMAND;
        current_row.SME_THROTL_MaxSpeed = state.smeThrottleDemand.MAX_SPEED;
        current_row.SME_THROTL_Forward = state.smeThrottleDemand.FORWARD;
        current_row.SME_THROTL_Reverse = state.smeThrottleDemand.REVERSE;
        current_row.SME_THROTL_PowerReady = state.smeThrottleDemand.POWER_READY;
        current_row.SME_THROTL_MBB_Alive = state.smeThrottleDemand.MBB_ALIVE;

        current_row.SME_CURRLIM_ChargeCurrentLim = state.smeMaxCurrents.CHARGE_CURRENT;
        current_row.SME_CURRLIM_DischargeCurrentLim = state.smeMaxCurrents.DISCHARGE_CURRENT;

        current_row.SME_TRQSPD_Speed = state.smeTrqSpd.SPEED;
        current_row.SME_TRQSPD_Torque = state.smeTrqSpd.TORQUE;
        current_row.SME_TRQSPD_SOC_Low_Traction = state.smeTrqSpd.SOC_LOW_TRACTION;
        current_row.SME_TRQSPD_SOC_Low_Hydraulic = state.smeTrqSpd.SOC_LOW_HYDRAULIC;
        current_row.SME_TRQSPD_Reverse = state.smeTrqSpd.REVERSE;
        current_row.SME_TRQSPD_Forward = state.smeTrqSpd.FORWARD;
        current_row.SME_TRQSPD_Park_Brake = state.smeTrqSpd.PARK_BRAKE;
        current_row.SME_TRQSPD_Pedal_Brake = state.smeTrqSpd.PEDAL_BRAKE;
        current_row.SME_TRQSPD_Controller_Overtermp = state.smeTrqSpd.CONTROLLER_OVERTEMP;
        current_row.SME_TRQSPD_Key_switch_overvolt = state.smeTrqSpd.KEY_SWITCH_OVERVOLT;
        current_row.SME_TRQSPD_Key_switch_undervolt = state.smeTrqSpd.KEY_SWITCH_UNDERVOLT;
        current_row.SME_TRQSPD_Running = state.smeTrqSpd.RUNNING;
        current_row.SME_TRQSPD_Traction = state.smeTrqSpd.TRACTION;
        current_row.SME_TRQSPD_Hydraulic = state.smeTrqSpd.HYDRAULIC;
        current_row.SME_TRQSPD_Powering_Enabled = state.smeTrqSpd.POWERING_ENABLED;
        current_row.SME_TRQSPD_Powering_Ready = state.smeTrqSpd.POWERING_READY;
        current_row.SME_TRQSPD_Precharging = state.smeTrqSpd.PRECHARGING;
        current_row.SME_TRQSPD_contactor_closed = state.smeTrqSpd.CONTACTOR_CLOSED;
        current_row.SME_TRQSPD_MotorFlags = state.smeTrqSpd.MOTOR_FLAGS;

        current_row.SME_TEMP_MotorTemperature = state.smeTemp.MOTOR_TEMP;
        current_row.SME_TEMP_ControllerTemperature = state.smeTemp.CONTROLLER_TEMP;
        current_row.SME_TEMP_DC_Bus_V = state.smeTemp.DC_BUS_V;
        current_row.SME_TEMP_FaultCode = state.smeTemp.FAULT_CODE;
        current_row.SME_TEMP_FaultLevel = state.smeTemp.FAULT_LEVEL;
        current_row.SME_TEMP_BusCurrent = state.smeTemp.BUS_CURRENT;

        // ACC
        current_row.ACC_STATUS_BMS_FAULT = state.accStatus.BMS_FAULT;
        current_row.ACC_STATUS_IMD_FAULT = state.accStatus.IMD_FAULT;
        current_row.ACC_STATUS_SHUTDOWN_STATE = state.accStatus.SHUTDOWN_STATE;
        current_row.ACC_STATUS_PRECHARGE_DONE = state.accStatus.PRECHARGE_DONE;
        current_row.ACC_STATUS_PRECHARGING = state.accStatus.PRECHARGING;
        current_row.ACC_STATUS_CHARGING = state.accStatus.CHARGING;
        current_row.ACC_STATUS_CELL_TOO_LOW = state.accStatus.CELL_TOO_LOW;
        current_row.ACC_STATUS_CELL_TOO_HIGH = state.accStatus.CELL_TOO_HIGH;
        current_row.ACC_STATUS_TEMP_TOO_LOW = state.accStatus.TEMP_TOO_LOW;
        current_row.ACC_STATUS_TEMP_TOO_HIGH = state.accStatus.TEMP_TOO_HIGH;
        current_row.ACC_STATUS_TEMP_TOO_HIGH_CRG = state.accStatus.TEMP_TOO_HIGH_CRG;
        current_row.ACC_STATUS_BALANCING = 0;
        current_row.ACC_STATUS_GLV_VOLTAGE = state.accStatus.GLV_VOLTAGE;
        current_row.ACC_STATUS_CELL_FAULT_INDEX = state.accStatus.CELL_FAULT_INDEX;

        current_row.ACC_POWER_PACK_VOLTAGE = state.accPower.PACK_VOLTAGE;
        current_row.ACC_POWER_SOC = state.accPower.SOC;
        current_row.ACC_POWER_CURRENT = state.accPower.CURRENT;

        current_row.ACC_SEG0_VOLTS_CELL0 = state.accSegVolts[0].VOLTS_CELL0;
        current_row.ACC_SEG0_VOLTS_CELL1 = state.accSegVolts[0].VOLTS_CELL1;
        current_row.ACC_SEG0_VOLTS_CELL2 = state.accSegVolts[0].VOLTS_CELL2;
        current_row.ACC_SEG0_VOLTS_CELL3 = state.accSegVolts[0].VOLTS_CELL3;
        current_row.ACC_SEG0_VOLTS_CELL4 = state.accSegVolts[0].VOLTS_CELL4;
        current_row.ACC_SEG0_VOLTS_CELL5 = state.accSegVolts[0].VOLTS_CELL5;

        current_row.ACC_SEG0_TEMPS_CELL0 = state.accSegTemps[0].TEMPS_CELL0;
        current_row.ACC_SEG0_TEMPS_CELL1 = state.accSegTemps[0].TEMPS_CELL1;
        current_row.ACC_SEG0_TEMPS_CELL2 = state.accSegTemps[0].TEMPS_CELL2;
        current_row.ACC_SEG0_TEMPS_CELL3 = state.accSegTemps[0].TEMPS_CELL3;
        current_row.ACC_SEG0_TEMPS_CELL4 = state.accSegTemps[0].TEMPS_CELL4;
        current_row.ACC_SEG0_TEMPS_CELL5 = state.accSegTemps[0].TEMPS_CELL5;

        current_row.ACC_SEG1_VOLTS_CELL0 = state.accSegVolts[1].VOLTS_CELL0;
        current_row.ACC_SEG1_VOLTS_CELL1 = state.accSegVolts[1].VOLTS_CELL1;
        current_row.ACC_SEG1_VOLTS_CELL2 = state.accSegVolts[1].VOLTS_CELL2;
        current_row.ACC_SEG1_VOLTS_CELL3 = state.accSegVolts[1].VOLTS_CELL3;
        current_row.ACC_SEG1_VOLTS_CELL4 = state.accSegVolts[1].VOLTS_CELL4;
        current_row.ACC_SEG1_VOLTS_CELL5 = state.accSegVolts[1].VOLTS_CELL5;

        current_row.ACC_SEG1_TEMPS_CELL0 = state.accSegTemps[1].TEMPS_CELL0;
        current_row.ACC_SEG1_TEMPS_CELL1 = state.accSegTemps[1].TEMPS_CELL1;
        current_row.ACC_SEG1_TEMPS_CELL2 = state.accSegTemps[1].TEMPS_CELL2;
        current_row.ACC_SEG1_TEMPS_CELL3 = state.accSegTemps[1].TEMPS_CELL3;
        current_row.ACC_SEG1_TEMPS_CELL4 = state.accSegTemps[1].TEMPS_CELL4;
        current_row.ACC_SEG1_TEMPS_CELL5 = state.accSegTemps[1].TEMPS_CELL5;

        current_row.ACC_SEG2_VOLTS_CELL0 = state.accSegVolts[2].VOLTS_CELL0;
        current_row.ACC_SEG2_VOLTS_CELL1 = state.accSegVolts[2].VOLTS_CELL1;
        current_row.ACC_SEG2_VOLTS_CELL2 = state.accSegVolts[2].VOLTS_CELL2;
        current_row.ACC_SEG2_VOLTS_CELL3 = state.accSegVolts[2].VOLTS_CELL3;
        current_row.ACC_SEG2_VOLTS_CELL4 = state.accSegVolts[2].VOLTS_CELL4;
        current_row.ACC_SEG2_VOLTS_CELL5 = state.accSegVolts[2].VOLTS_CELL5;

        current_row.ACC_SEG2_TEMPS_CELL0 = state.accSegTemps[2].TEMPS_CELL0;
        current_row.ACC_SEG2_TEMPS_CELL1 = state.accSegTemps[2].TEMPS_CELL1;
        current_row.ACC_SEG2_TEMPS_CELL2 = state.accSegTemps[2].TEMPS_CELL2;
        current_row.ACC_SEG2_TEMPS_CELL3 = state.accSegTemps[2].TEMPS_CELL3;
        current_row.ACC_SEG2_TEMPS_CELL4 = state.accSegTemps[2].TEMPS_CELL4;
        current_row.ACC_SEG2_TEMPS_CELL5 = state.accSegTemps[2].TEMPS_CELL5;

        current_row.ACC_SEG3_VOLTS_CELL0 = state.accSegVolts[3].VOLTS_CELL0;
        current_row.ACC_SEG3_VOLTS_CELL1 = state.accSegVolts[3].VOLTS_CELL1;
        current_row.ACC_SEG3_VOLTS_CELL2 = state.accSegVolts[3].VOLTS_CELL2;
        current_row.ACC_SEG3_VOLTS_CELL3 = state.accSegVolts[3].VOLTS_CELL3;
        current_row.ACC_SEG3_VOLTS_CELL4 = state.accSegVolts[3].VOLTS_CELL4;
        current_row.ACC_SEG3_VOLTS_CELL5 = state.accSegVolts[3].VOLTS_CELL5;

        current_row.ACC_SEG3_TEMPS_CELL0 = state.accSegTemps[3].TEMPS_CELL0;
        current_row.ACC_SEG3_TEMPS_CELL1 = state.accSegTemps[3].TEMPS_CELL1;
        current_row.ACC_SEG3_TEMPS_CELL2 = state.accSegTemps[3].TEMPS_CELL2;
        current_row.ACC_SEG3_TEMPS_CELL3 = state.accSegTemps[3].TEMPS_CELL3;
        current_row.ACC_SEG3_TEMPS_CELL4 = state.accSegTemps[3].TEMPS_CELL4;
        current_row.ACC_SEG3_TEMPS_CELL5 = state.accSegTemps[3].TEMPS_CELL5;

        current_row.ACC_SEG4_VOLTS_CELL0 = state.accSegVolts[4].VOLTS_CELL0;
        current_row.ACC_SEG4_VOLTS_CELL1 = state.accSegVolts[4].VOLTS_CELL1;
        current_row.ACC_SEG4_VOLTS_CELL2 = state.accSegVolts[4].VOLTS_CELL2;
        current_row.ACC_SEG4_VOLTS_CELL3 = state.accSegVolts[4].VOLTS_CELL3;
        current_row.ACC_SEG4_VOLTS_CELL4 = state.accSegVolts[4].VOLTS_CELL4;
        current_row.ACC_SEG4_VOLTS_CELL5 = state.accSegVolts[4].VOLTS_CELL5;

        current_row.ACC_SEG4_TEMPS_CELL0 = state.accSegTemps[4].TEMPS_CELL0;
        current_row.ACC_SEG4_TEMPS_CELL1 = state.accSegTemps[4].TEMPS_CELL1;
        current_row.ACC_SEG4_TEMPS_CELL2 = state.accSegTemps[4].TEMPS_CELL2;
        current_row.ACC_SEG4_TEMPS_CELL3 = state.accSegTemps[4].TEMPS_CELL3;
        current_row.ACC_SEG4_TEMPS_CELL4 = state.accSegTemps[4].TEMPS_CELL4;
        current_row.ACC_SEG4_TEMPS_CELL5 = state.accSegTemps[4].TEMPS_CELL5;

        // ETC
        current_row.ETC_STATUS_HE1 = state.etcStatus.HE1;
        current_row.ETC_STATUS_HE2 = state.etcStatus.HE2;
        current_row.ETC_STATUS_BRAKE_SENSE_VOLTAGE = state.etcStatus.BRAKE_SENSE_VOLTAGE;
        current_row.ETC_STATUS_PEDAL_TRAVEL = state.etcStatus.PEDAL_TRAVEL;
        current_row.ETC_STATUS_RTD_BUTTON = state.etcStatus.RTD_BUTTON;
        current_row.ETC_STATUS_RTDS = state.etcStatus.RTDS;
        current_row.ETC_STATUS_REVERSE = state.etcStatus.REVERSE;
        current_row.ETC_STATUS_BRAKELIGHT = state.etcStatus.BRAKELIGHT;
        current_row.ETC_STATUS_RTD = state.etcStatus.RTD;
        current_row.ETC_STATUS_IMPLAUSIBILITY = state.etcStatus.IMPLAUSIBILITY;
        current_row.ETC_STATUS_TS_ACTIVE = state.etcStatus.TS_ACTIVE;

        // PDB
        current_row.PDB_POWER_A_GLV_VOLTAGE = state.pdbPowerA.GLV_VOLTAGE;
        current_row.PDB_POWER_A_CURRENT_SHUTDOWN = state.pdbPowerA.CURRENT_SHUTDOWN;
        current_row.PDB_POWER_A_CURRENT_ACC = state.pdbPowerA.CURRENT_ACC;
        current_row.PDB_POWER_A_CURRENT_ETC = state.pdbPowerA.CURRENT_ETC;
        current_row.PDB_POWER_A_CURRENT_BPS = state.pdbPowerA.CURRENT_BPS;
        current_row.PDB_POWER_A_CURRENT_TRACTIVE = state.pdbPowerA.CURRENT_TRACTIVE;
        current_row.PDB_POWER_A_CURRENT_BSPD = state.pdbPowerA.CURRENT_BSPD;

        current_row.PDB_POWER_B_CURRENT_TELEMETRY = state.pdbPowerB.CURRENT_TELEMETRY;
        current_row.PDB_POWER_B_CURRENT_PDB = state.pdbPowerB.CURRENT_PDB;
        current_row.PDB_POWER_B_CURRENT_DASH = state.pdbPowerB.CURRENT_DASH;
        current_row.PDB_POWER_B_CURRENT_RTML = state.pdbPowerB.CURRENT_RTML;
        current_row.PDB_POWER_B_CURRENT_EXTRA_1 = state.pdbPowerB.CURRENT_EXTRA_1;
        current_row.PDB_POWER_B_CURRENT_EXTRA_2 = state.pdbPowerB.CURRENT_EXTRA_2;

        // TMAIN
        current_row.TMAIN_DATA_BRAKES_F = state.brake_sensor_f;
        current_row.TMAIN_DATA_BRAKES_R = state.brake_sensor_r;

        // TPERIPH
        current_row.TPERIPH_FL_DATA_WHEELSPEED = state.tperiphData[0].WHEELSPEED;
        current_row.TPERIPH_FL_DATA_SUSTRAVEL = state.tperiphData[0].SUSTRAVEL;
        current_row.TPERIPH_FL_DATA_STRAIN = state.tperiphData[0].STRAIN;
        current_row.TPERIPH_FL_DATA_SIDE_TIRE_TEMP = state.tperiphData[0].SIDE_TIRE_TEMP;

        current_row.TPERIPH_FR_DATA_WHEELSPEED = state.tperiphData[1].WHEELSPEED;
        current_row.TPERIPH_FR_DATA_SUSTRAVEL = state.tperiphData[1].SUSTRAVEL;
        current_row.TPERIPH_FR_DATA_STRAIN = state.tperiphData[1].STRAIN;
        current_row.TPERIPH_FR_DATA_SIDE_TIRE_TEMP = state.tperiphData[1].SIDE_TIRE_TEMP;

        current_row.TPERIPH_BL_DATA_WHEELSPEED = state.tperiphData[2].WHEELSPEED;
        current_row.TPERIPH_BL_DATA_SUSTRAVEL = state.tperiphData[2].SUSTRAVEL;
        current_row.TPERIPH_BL_DATA_STRAIN = state.tperiphData[2].STRAIN;
        current_row.TPERIPH_BL_DATA_SIDE_TIRE_TEMP = state.tperiphData[2].SIDE_TIRE_TEMP;

        current_row.TPERIPH_BR_DATA_WHEELSPEED = state.tperiphData[3].WHEELSPEED;
        current_row.TPERIPH_BR_DATA_SUSTRAVEL = state.tperiphData[3].SUSTRAVEL;
        current_row.TPERIPH_BR_DATA_STRAIN = state.tperiphData[3].STRAIN;
        current_row.TPERIPH_BR_DATA_SIDE_TIRE_TEMP = state.tperiphData[3].SIDE_TIRE_TEMP;

        current_row.TPERIPH_FL_TIRETEMP_1 = state.tperiphTireTemp[0].TIRETEMP_1;
        current_row.TPERIPH_FL_TIRETEMP_1 = state.tperiphTireTemp[0].TIRETEMP_2;
        current_row.TPERIPH_FL_TIRETEMP_1 = state.tperiphTireTemp[0].TIRETEMP_3;
        current_row.TPERIPH_FL_TIRETEMP_1 = state.tperiphTireTemp[0].TIRETEMP_4;
        current_row.TPERIPH_FL_TIRETEMP_1 = state.tperiphTireTemp[0].TIRETEMP_5;
        current_row.TPERIPH_FL_TIRETEMP_1 = state.tperiphTireTemp[0].TIRETEMP_6;
        current_row.TPERIPH_FL_TIRETEMP_1 = state.tperiphTireTemp[0].TIRETEMP_7;
        current_row.TPERIPH_FL_TIRETEMP_1 = state.tperiphTireTemp[0].TIRETEMP_8;

        current_row.TPERIPH_FL_TIRETEMP_2 = state.tperiphTireTemp[1].TIRETEMP_1;
        current_row.TPERIPH_FL_TIRETEMP_2 = state.tperiphTireTemp[1].TIRETEMP_2;
        current_row.TPERIPH_FL_TIRETEMP_2 = state.tperiphTireTemp[1].TIRETEMP_3;
        current_row.TPERIPH_FL_TIRETEMP_2 = state.tperiphTireTemp[1].TIRETEMP_4;
        current_row.TPERIPH_FL_TIRETEMP_2 = state.tperiphTireTemp[1].TIRETEMP_5;
        current_row.TPERIPH_FL_TIRETEMP_2 = state.tperiphTireTemp[1].TIRETEMP_6;
        current_row.TPERIPH_FL_TIRETEMP_2 = state.tperiphTireTemp[1].TIRETEMP_7;
        current_row.TPERIPH_FL_TIRETEMP_2 = state.tperiphTireTemp[1].TIRETEMP_8;

        current_row.TPERIPH_FL_TIRETEMP_3 = state.tperiphTireTemp[2].TIRETEMP_1;
        current_row.TPERIPH_FL_TIRETEMP_3 = state.tperiphTireTemp[2].TIRETEMP_2;
        current_row.TPERIPH_FL_TIRETEMP_3 = state.tperiphTireTemp[2].TIRETEMP_3;
        current_row.TPERIPH_FL_TIRETEMP_3 = state.tperiphTireTemp[2].TIRETEMP_4;
        current_row.TPERIPH_FL_TIRETEMP_3 = state.tperiphTireTemp[2].TIRETEMP_5;
        current_row.TPERIPH_FL_TIRETEMP_3 = state.tperiphTireTemp[2].TIRETEMP_6;
        current_row.TPERIPH_FL_TIRETEMP_3 = state.tperiphTireTemp[2].TIRETEMP_7;
        current_row.TPERIPH_FL_TIRETEMP_3 = state.tperiphTireTemp[2].TIRETEMP_8;

        current_row.TPERIPH_FL_TIRETEMP_4 = state.tperiphTireTemp[3].TIRETEMP_1;
        current_row.TPERIPH_FL_TIRETEMP_4 = state.tperiphTireTemp[3].TIRETEMP_2;
        current_row.TPERIPH_FL_TIRETEMP_4 = state.tperiphTireTemp[3].TIRETEMP_3;
        current_row.TPERIPH_FL_TIRETEMP_4 = state.tperiphTireTemp[3].TIRETEMP_4;
        current_row.TPERIPH_FL_TIRETEMP_4 = state.tperiphTireTemp[3].TIRETEMP_5;
        current_row.TPERIPH_FL_TIRETEMP_4 = state.tperiphTireTemp[3].TIRETEMP_6;
        current_row.TPERIPH_FL_TIRETEMP_4 = state.tperiphTireTemp[3].TIRETEMP_7;
        current_row.TPERIPH_FL_TIRETEMP_4 = state.tperiphTireTemp[3].TIRETEMP_8;

        // VDM
        current_row.VDM_GPS_Latitude = state.vdmGpsLatLong.LATITUDE;
        current_row.VDM_GPS_Longitude = state.vdmGpsLatLong.LONGITUDE;

        current_row.VDM_GPS_SPEED = state.vdmGpsData.SPEED;
        current_row.VDM_GPS_ALTITUDE = state.vdmGpsData.ALTITUDE;
        current_row.VDM_GPS_TRUE_COURSE = state.vdmGpsData.TRUE_COURSE;
        current_row.VDM_GPS_SATELLITES_IN_USE = state.vdmGpsData.SATELLITES_IN_USE;
        current_row.VDM_GPS_VALID1 = state.vdmGpsData.VALID1;

        current_row.VDM_UTC_DATE_YEAR = state.vdmDateTime.UTC_DATE_YEAR;
        current_row.VDM_UTC_DATE_MONTH = state.vdmDateTime.UTC_DATE_MONTH;
        current_row.VDM_UTC_DATE_DAY = state.vdmDateTime.UTC_DATE_DAY;
        current_row.VDM_UTC_TIME_HOURS = state.vdmDateTime.UTC_TIME_HOURS;
        current_row.VDM_UTC_TIME_MINUTES = state.vdmDateTime.UTC_TIME_MINUTES;
        current_row.VDM_UTC_TIME_SECONDS = state.vdmDateTime.UTC_TIME_SECONDS;
        current_row.VDM_GPS_VALID2 = state.vdmDateTime.VALID2;

        current_row.VDM_X_AXIS_ACCELERATION = state.vdmAcceleration.X;
        current_row.VDM_Y_AXIS_ACCELERATION = state.vdmAcceleration.Y;
        current_row.VDM_Z_AXIS_ACCELERATION = state.vdmAcceleration.Z;

        current_row.VDM_X_AXIS_YAW_RATE = state.vdmYawRate.X;
        current_row.VDM_Y_AXIS_YAW_RATE = state.vdmYawRate.Y;
        current_row.VDM_Z_AXIS_YAW_RATE = state.vdmYawRate.Z;

        // current_row.COMMAND_COMMAND_SPECIFIER
        // current_row.COMMAND_MODE
        // current_row.RESPONSE_COMMAND_SPECIFIER
        // current_row.SMPC_CONTROL_DEST_NODE_ID
        // current_row.SMPC_CONTROL_ENABLE
        // current_row.SMPC_CONTROL_CURRENT_10X_MULT
        // current_row.SMPC_CONTROL_EVSE_OVERRIDE
        // current_row.SMPC_CONTROL_PACK_VOLTAGE
        // current_row.SMPC_MAX_DEST_NODE_ID
        // current_row.SMPC_MAX_CHRG_VOLTAGE
        // current_row.SMPC_MAX_CHRG_CURRENT
        // current_row.SMPC_MAX_INPUT_EVSE_OVERRIDE
        // current_row.SMPC_STATUS_READY
        // current_row.SMPC_STATUS_CHARGE_COMPLETE
        // current_row.SMPC_STATUS_OVERTEMP_FAULT
        // current_row.SMPC_STATUS_OVERCURRENT_FAULT
        // current_row.SMPC_STATUS_OVERVOLTAGE_FAULT
        // current_row.SMPC_STATUS_UNDERVOLTAGE_FAULT
        // current_row.SMPC_STATUS_INTERNAL_FAULT
        // current_row.SMPC_STATUS_CHARGING
        // current_row.SMPC_STATUS_CHARGER_UNPLUGGED
        // current_row.SMPC_STATUS_REQUEST_EXCLUSIVE
        // current_row.SMPC_STATUS_J1772_DISCONN
        // current_row.SMPC_STATUS_VOLTAGE_MV
        // current_row.SMPC_STATUS_CURRENT_MA
        // current_row.SMPC_MAX2_MAX_VOLTAGE_MV
        // current_row.SMPC_MAX2_MAX_CURRENT_MA
        // current_row.SMPC_MAX2_EVSE_CURRENT
        // current_row.SMPC_MAX2_INPUT_CURR_LIM
        // current_row.SMPC_INPUT_AC_INPUT_VOLTAGE
        // current_row.SMPC_INPUT_J1772_CONNECTED
        // current_row.SMPC_INPUT_J1772_DISCONNECTED
        // current_row.SMPC_INPUT_J1772_TRIGGERED
        // current_row.SMPC_INPUT_AC_INPUT_CURRENT_MA
        // current_row.SMPC_INPUT_AC_INPUT_FREQUENCY_HZ
        // current_row.SMPC_INPUT_MAX_CHARGER_TEMP_C
        // current_row.SMPC_SER_PART_NUMBER
        // current_row.SMPC_SER_SERIAL_NUMBER
        // current_row.SMPC_SER_FIRMWARE_VER
    });
    queue.call_every(100ms, []() { n++; });
    queue.dispatch_forever();
}
