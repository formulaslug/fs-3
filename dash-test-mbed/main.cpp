#include "main.h"
#include "rtos.h"
#include "bt817/address_mapping.h"
#include "bt817/bt817_api.h"
#include "bt817/bt817_init.h"
#include "bt817/dl_commands.h"

SPI spi(PC_12, PC_11, PC_10);


// SPI spi(D12, D11, D9);
// DigitalOut led(LED1);
DigitalOut dash_cs(PD_2, 1);
DigitalOut dash_pd(PB_7, 1);

void test();

int main()
{
    dash_pd = 1;
    ThisThread::sleep_for(20ms);
    dash_pd = 0;
    ThisThread::sleep_for(6ms);
    dash_pd = 1;
    ThisThread::sleep_for(21ms);
    //init()
    bt817_init();
    //test
    test();
   while(true) {
       test();
       // led = !led;
       ThisThread::sleep_for(400ms);
   }

   // main() is expected to loop forever.
   // If main() actually returns the processor will halt
   return 0;
}

void test() {
    cmd( COLOR_RGB(160, 22, 22)); // change colour to red
    cmd(
         POINT_SIZE(320)); // set point size to 20 pixels in radius
    cmd( BEGIN(POINTS));             // start drawing points
    cmd( VERTEX2II(192, 133, 0, 0)); // red point
    cmd( END());
    cmd( DISPLAY()); // display the image
    // DBG_A("Successfully added point to DL, swapping frame\r\n");
    wr8(REG_DLSWAP, DLSWAP_FRAME);
    // DBG_A("Swapped frame\r\n");
}
