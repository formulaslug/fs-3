#include "mbed.h"
int main() {
    printf("program ran\n");
    CAN* canBus;
    canBus = new CAN (PA_11, PA_12, 500000);

    struct acc_messages {
        u_int cell0;
        u_int cell1;
        u_int cell2;
        u_int cell3;
        u_int cell4;
        u_int cell5;
    };
    acc_messages accData;

    struct peripheral_messages {
        signed int wheelSpeed;
        u_int suspensionTravel;
        signed int strain;
        u_int tireTemp;
    };

    peripheral_messages BL;
    peripheral_messages BR;
    peripheral_messages FR;
    peripheral_messages FL;
    struct main_message {
        u_int brakesFr;
        u_int brakesBa;
    };

    main_message mainData;

    while (true) {
        CANMessage msg;
        canBus->read(msg);
        switch (int id = msg.id) {
            case 0x291: case 0x292: case 0x293: case 0x294: case 0x295:
                printf("ID: %x, Cell Temp 0: %d, Cell Temp 1: %d, Cell Temp 2: %d, Cell Temp 3: %d, Cell Temp 4: %d, Cell Temp 5: %d"
                , id, msg.data[0], msg.data[1] << 8, msg.data[2] << 16, msg.data[3] << 24, msg.data[4] << 32);
                msg.data[0] = accData.cell0;
                msg.data[1] = accData.cell1 >> 8;
                msg.data[2] = accData.cell2 >> 16;
                msg.data[3] = accData.cell3 >> 24;
                msg.data[4] = accData.cell4 >> 32;
                break;
            case 0x1A0:
                printf("FBr: %x, RBr: %x", msg.data[0], msg.data[1] << 16);
                mainData.brakesFr = msg.data[0];
                mainData.brakesBa = msg.data[1] >> 16;
                break;
            case 0x1A4:
                printf("BL: WS: %x, SusTr %x, Strain %x, TTemp %x", msg.data[0], msg.data[1] << 16, msg.data[2] << 32, msg.data[3] << 48);;
                BL.wheelSpeed = msg.data[0];
                BL.suspensionTravel = msg.data[1] >> 16;
                BL.strain = msg.data[2] >> 32;
                BL.tireTemp = msg.data[3] >> 48;
                break;
            case 0x1A5:
                printf("BR: WS: %x, SusTr %x, Strain %x, TTemp %x", msg.data[0], msg.data[1] << 16, msg.data[2] << 32, msg.data[3] << 48);;
                BR.wheelSpeed = msg.data[0];
                BR.suspensionTravel = msg.data[1] >> 16;
                BR.strain = msg.data[2] >> 32;
                BR.tireTemp = msg.data[3] >> 48;
                break;
            case 0x1A2:
                printf("FL: WS: %x, SusTr %x, Strain %x, TTemp %x", msg.data[0], msg.data[1] << 16, msg.data[2] << 32, msg.data[3] << 48);;
                FL.wheelSpeed = msg.data[0];
                FL.suspensionTravel = msg.data[1] >> 16;
                FL.strain = msg.data[2] >> 32;
                FL.tireTemp = msg.data[3] >> 48;
                break;
            case 0x1A3:
                printf("FR: WS: %x, SusTr %x, Strain %x, TTemp %x", msg.data[0], msg.data[1] << 16, msg.data[2] << 32, msg.data[3] << 48);;
                FR.wheelSpeed = msg.data[0];
                FR.suspensionTravel = msg.data[1] >> 16;
                FR.strain = msg.data[2] >> 32;
                FR.tireTemp = msg.data[3] >> 48;
                break;
            default:
                printf("Unknown ID: %x\n", id);
                printf("Fail");
        }
        ThisThread::sleep_for(100ms);
    }
}