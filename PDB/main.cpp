#include "mbed.h"
#include <vector>
#include <numeric>
#include <map>

// DigitalOut dcdc_ctrl(PB_5);
DigitalOut reset_ctrl(PA_1);
AnalogIn lv_batt(PA_0);
DigitalOut S0(PB_6);
DigitalOut S1(PB_7);
DigitalOut S2(PA_10);
DigitalOut S3(PA_9);
AnalogIn viout(PB_0);

CAN* can;

std::map<std::string, std::array<int, 4>> mux;

vector<float> viout_buffer;

float glv;
float glv_trunc;
float rtm_curr;
float bps_curr;
float bspd_curr;
float pdb_curr;
float telem_curr;
float shutdown_curr;
float tractive_curr;
float ex2_curr;
float acc_curr;
float dash_curr;
float ex1_curr;
float etc_curr;

void send_PDB_TPDO_POWER_A() {
    CANMessage msg;
    msg.id = 0x19a;
    msg.data[0] = glv*1000;
    msg.data[1] = (uint16_t)(glv*1000) >> 8;
    msg.data[2] = (uint8_t)(shutdown_curr * 10);
    msg.data[3] = (uint8_t)(acc_curr * 10);
    msg.data[4] = (uint8_t)(etc_curr * 10);
    msg.data[5] = (uint8_t)(bps_curr*10);
    msg.data[6] = (uint8_t)(tractive_curr*10);
    msg.data[7] = (uint8_t)(bspd_curr*10);
    can->write(msg);
    printf("CAN A sent\n");
}

void send_PDB_TPDO_POWER_B() {
    CANMessage msg;
    msg.id = 0x29a;
    msg.data[0] = (uint8_t)(telem_curr*10);
    msg.data[1] = (uint8_t)(pdb_curr*10);
    msg.data[2] = (uint8_t)(dash_curr*10);
    msg.data[3] = (uint8_t)(rtm_curr*10);
    msg.data[4] = (uint8_t)(ex1_curr*10);
    msg.data[5] = (uint8_t)(ex2_curr*10);
    can->write(msg);
    printf("CAN B sent\n");
}

int main(){
    printf("entering main\n");

    can = new CAN(PA_11, PA_12, 500000);

    mux["rtm"] = {0,0,0,1};
    mux["bps"] = {0,0,1,0};
    mux["bspd"] = {0,0,1,1};
    mux["pdb"] = {0,1,0,0};
    mux["telem"] = {0,1,0,1};
    mux["shutdown"] = {0,1,1,1};
    mux["tractive"] = {1,0,0,0};
    mux["ex2"] = {1,0,0,1};
    mux["acc"] = {1,0,1,0};
    mux["dash"] = {1,0,1,1};
    mux["ex1"] = {1,1,0,1};
    mux["etc"] = {1,1,1,0};

    while (true) {
        glv = lv_batt.read();
        glv_trunc = round(glv * 100 + 0.5) / 100;
        printf("glv battery = %f\n", glv_trunc);
        int canAcounter = 1;
        int canBcounter = 0;
        for (auto[sensor, sBits] : mux) {
            reset_ctrl = 0;
            int i = 0;
            float viout_avg;
            while (i < 10) {
                S0 = sBits[3];
                S1 = sBits[2];
                S2 = sBits[1];
                S3 = sBits[0];

                viout_buffer.push_back((viout.read()*3.3-1.65)/0.132);
                if (viout_buffer.size() > 10) {
                    viout_buffer.erase(viout_buffer.begin());
                }
                ThisThread::sleep_for(60ms);
                i += 1;
            }

            viout_avg = std::reduce(viout_buffer.begin(), viout_buffer.end()) / viout_buffer.size();
            float viout_avg_trunc = round(viout_avg * 100 + 0.5) / 100;


            if (sensor == "rtm"){
                rtm_curr = viout_avg_trunc;
                printf("rtm current = %f\n", rtm_curr);
                canBcounter++;

            } else if (sensor == "bps") {
                bps_curr = viout_avg_trunc;
                printf("bps current = %f\n", bps_curr);
                canAcounter += 1;

            } else if (sensor == "bspd") {
                bspd_curr = viout_avg_trunc;
                printf("bspd current = %f\n", bspd_curr);
                canAcounter += 1;

            } else if (sensor == "pdb") {
                pdb_curr = viout_avg_trunc;
                printf("pdb current = %f\n", pdb_curr);
                canBcounter++;

            } else if (sensor == "telem") {
                telem_curr = viout_avg_trunc;
                printf("telem current = %f\n", telem_curr);
                canBcounter++;

            } else if (sensor == "shutdown") {
                shutdown_curr = viout_avg_trunc;
                printf("shutdown current = %f\n", shutdown_curr);
                canAcounter+=1;

            } else if (sensor == "tractive") {
                tractive_curr = viout_avg_trunc;
                printf("tractive current = %f\n", tractive_curr);
                canAcounter+=1;

            } else if (sensor == "ex2") {
                ex2_curr = viout_avg_trunc;
                printf("extra 2 current = %f\n", ex2_curr);
                canBcounter+=1;

            } else if (sensor == "acc") {
                acc_curr = viout_avg_trunc;
                printf("acc current = %f\n", acc_curr);
                canAcounter+=1;

            } else if (sensor == "dash") {
                dash_curr = viout_avg_trunc;
                printf("dash current = %f\n", dash_curr);
                canBcounter+=1;

            } else if (sensor == "ex1") {
                ex1_curr = viout_avg_trunc;
                printf("extra 1 current = %f\n", ex1_curr);
                canBcounter+=1;

            } else if (sensor == "etc") {
                etc_curr = viout_avg_trunc;
                printf("etc current = %f\n", etc_curr);
                canAcounter+=1;
            }
        }
        if (canAcounter == 7) {
            send_PDB_TPDO_POWER_A();
        } else {
            printf("canAcounter = %d\n", canAcounter);
        }

        if (canBcounter == 6) {
            send_PDB_TPDO_POWER_B();
        } else {
            printf("canBcounter = %d\n", canBcounter);
        }
    }
}