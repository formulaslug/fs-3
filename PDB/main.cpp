#include "mbed.h"
#include <map>
#include <numeric>
#include <vector>

DigitalOut dcdc_ctrl(PB_5);
DigitalOut reset_ctrl(PA_1);
AnalogIn lv_sense(PA_0);
DigitalOut S0(PB_6);
DigitalOut S1(PB_7);
DigitalOut S2(PA_10);
DigitalOut S3(PA_9);
AnalogIn viout(PB_0);

CAN* can;

EventQueue queue(32 * EVENTS_EVENT_SIZE);

uint16_t glv;
float lv_sense_val;
const float LV_SENSE_FACTOR = 5.0f;   // depends on voltage divider
const float GLV_THRESHOLD = 11.0f;

vector<uint8_t> rtm_curr_vec;
vector<uint8_t> bps_curr_vec;
vector<uint8_t> bspd_curr_vec;
vector<uint8_t> pdb_curr_vec;
vector<uint8_t> telem_curr_vec;
vector<uint8_t> shutdown_curr_vec;
vector<uint8_t> tractive_curr_vec;
vector<uint8_t> ex2_curr_vec;
vector<uint8_t> acc_curr_vec;
vector<uint8_t> dash_curr_vec;
vector<uint8_t> ex1_curr_vec;
vector<uint8_t> etc_curr_vec;

uint8_t rtm_curr;
uint8_t bps_curr;
uint8_t bspd_curr;
uint8_t pdb_curr;
uint8_t telem_curr;
uint8_t shutdown_curr;
uint8_t tractive_curr;
uint8_t ex2_curr;
uint8_t acc_curr;
uint8_t dash_curr;
uint8_t ex1_curr;
uint8_t etc_curr;

enum DEVICES {
    RTM = 1,
    BPS = 2,
    BSPD = 3,
    PDB = 4,
    TELEM = 5,
    SHUTDOWN = 7,
    TRACTIVE = 8,
    EX2 = 9,
    ACC = 10,
    DASH = 11,
    EX1 = 13,
    ETC = 14
};

void send_PDB_TPDO_POWER_A();
void send_PDB_TPDO_POWER_B();

uint8_t read_current();

uint8_t process_current(std::vector<uint8_t> &vec);

void send_PDB_TPDO_POWER_A() {
    CANMessage msg;
    msg.id = 0x19a;
    msg.data[0] = glv;
    msg.data[1] = glv >> 8;
    msg.data[2] = shutdown_curr;
    msg.data[3] = acc_curr;
    msg.data[4] = etc_curr;
    msg.data[5] = bps_curr;
    msg.data[6] = tractive_curr;
    msg.data[7] = bspd_curr;
    can->write(msg);
    ThisThread::sleep_for(1ms);
    // printf("CAN A sent\n");
}

void send_PDB_TPDO_POWER_B() {
    CANMessage msg;
    msg.id = 0x29a;
    msg.data[0] = telem_curr;
    msg.data[1] = pdb_curr;
    msg.data[2] = dash_curr;
    msg.data[3] = rtm_curr;
    msg.data[4] = ex1_curr;
    msg.data[5] = ex2_curr;
    can->write(msg);
    ThisThread::sleep_for(1ms);
    // printf("CAN B sent\n");
}

int main() {
    printf("entering main\n");

    can = new CAN(PA_11, PA_12, 500000);
    can->filter(0x188, 0xFF, CANAny); // ACC_TPDO_STATUS TODO: does this even work?

    queue.call_every(200ms, send_PDB_TPDO_POWER_A);
    queue.call_every(200ms, send_PDB_TPDO_POWER_B);

    while (true) {
        lv_sense_val = lv_sense.read() * 3.3f * LV_SENSE_FACTOR;
        if (lv_sense_val < GLV_THRESHOLD) {
            reset_ctrl.write(1);
        }

        CANMessage msg;
        while (can->read(msg)) {
            if (msg.id == 0x188) // ACC_TPDO_STATUS
            {
                bool precharge_done = msg.data[0] & 0x08;
                dcdc_ctrl.write(precharge_done);
            }
        }

        for (int i = 0; i < 16; i++)
        {
            S0.write(i & 0x1);
            S1.write(i & 0x2);
            S2.write(i & 0x4);
            S2.write(i & 0x8);
            ThisThread::sleep_for(1ms);
            switch (i)
            {
            case RTM:
                rtm_curr = process_current(rtm_curr_vec);
                break;
            case BPS:
                bps_curr = process_current(bps_curr_vec);
                break;
            case BSPD:
                bspd_curr = process_current(bspd_curr_vec);
                break;
            case PDB:
                pdb_curr = process_current(pdb_curr_vec);
                break;
            case TELEM:
                telem_curr = process_current(telem_curr_vec);
                break;
            case SHUTDOWN:
                shutdown_curr = process_current(shutdown_curr_vec);
                break;
            case TRACTIVE:
                tractive_curr = process_current(tractive_curr_vec);
                break;
            case EX2:
                ex2_curr = process_current(ex2_curr_vec);
                break;
            case ACC:
                acc_curr = process_current(acc_curr_vec);
                break;
            case DASH:
                dash_curr = process_current(dash_curr_vec);
                break;
            case EX1:
                ex1_curr = process_current(ex1_curr_vec);
                break;
            case ETC:
                etc_curr = process_current(etc_curr_vec);
                break;
            }
        }
        queue.dispatch_once();
        ThisThread::sleep_for(1ms);
    }
}

uint8_t process_current(std::vector<uint8_t> &vec) {
    if (vec.size() > 5)
    {
        vec.erase(vec.begin());
    }
    vec.push_back(read_current());
    return std::accumulate(vec.begin(), vec.end(), 0) / vec.size();
}

uint8_t read_current() {
    // Read the current sensor and store the value as a uint8 [0,255]
    // Could do this with .read_uint16() but the bitshift would be different
    // depending on the ADC bitdepth and this is guaranteed to work.

    // TODO: Update DBC with Voltage-Current mappings for each sensor
    return static_cast<uint8_t>(viout.read() * 255);
}
