#include "mbed.h"
#include <vector>
#include <numeric>


// DigitalOut dcdc_ctrl(PB_5);
DigitalOut reset_ctrl(PA_1);
AnalogIn lv_batt(PA_0);
DigitalOut S0(PB_6);
DigitalOut S1(PB_7);
DigitalOut S2(PA_10);
DigitalOut S3(PA_9);
AnalogIn viout(PB_0);



#define rtm [0,0,0,1]
#define bps [0,0,1,0]
#define bspd [0,0,1,1]
#define pdb [0,1,0,0]
#define telem [0,1,0,1]
#define sh [0,1,1,1]
#define tractive [1,0,0,0]
#define acc [1,0,0,1]
#define dash [1,0,1,1]
#define extra1 [1,1,0,0]
#define etc [1,1,0,1]

vector<float> viout_buffer;

int main()
{
    int mux_add[4] = {0};


    mux_add = acc;
    reset_ctrl = 0;
    while (true)
    {
        S0 = mux_add[3];
        S1 = mux_add[2];
        S2 = mux_add[1];
        S3 = mux_add[0];

        viout_buffer.push_back(((viout.read()*3.3)-1.65)/0.132);
        if (viout_buffer.size() > 10) {
            viout_buffer.erase(viout_buffer.begin());
        }

        float viout_avg = std::reduce(viout_buffer.begin(), viout_buffer.end()) / viout_buffer.size();

        printf("rtm current = %fA\n", viout_avg);
        ThisThread::sleep_for(100ms);

    }
}