//
// Created by coles on 5/15/2025.
//

#ifndef CONFIG_H
#define CONFIG_H

#ifndef PIN_CAN1_TD
#define PIN_CAN1_TD PA_12
#endif


#ifndef PIN_CAN1_RD
#define PIN_CAN1_RD PA_11
#endif

#ifndef PIN_CONTROL_PILOT
#define PIN_CONTROL_PILOT PA_7
#endif

#ifndef PIN_PROXIMITY_PILOT
#define PIN_PROXIMITY_PILOT PA_6
#endif

#ifndef CAN_FREQUENCY
#define CAN_FREQUENCY 500000
#endif

#ifndef MAX_AC_CURRENT
#define MAX_AC_CURRENT 32
#endif

#ifndef VOLTAGE_TARGET_MV
#define VOLTAGE_TARGET_MV 120000
#endif

// in centiamps (0.01 amps)
#ifndef CURRENT_MAX_CA
#define CURRENT_MAX_CA 5200 // 52 amps is 1C for now, battery can support up to 120 amps according to datasheet, note that due to charger errors
#endif


#endif //CONFIG_H
