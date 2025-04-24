#include "bt817/bt817_init.h"

/*************************Private Include************************************/
#include "bt817/address_mapping.h"
#include "bt817/bt817_api.h"
#include "bt817/dl_commands.h"
#include "bt817/host_commands.h"
#include "main.h"

/*************************Private Define ************************************/
#define MHz_60 0x3938700 /* Default frequency of BT817 */
#define MHz_72 0x44AA200 /* Recommended higher frequency */

void bt817_init(void) {
	host_command(CLKEXT);
	// send command "CLKEXT" to use the external clock source
	host_command(CLKSEL);
	host_command(ACTIVE);
	HAL_Delay(300);
	volatile int dummy =
	    0; // to prevent compiler from optimizing away empty loops
	while (0x7C != rd8(REG_ID)) {
		dummy++; // empty while to pause execution until the right value
		         // is returned
	}
	while (0x0 != rd16(REG_CPURESET)) {
		dummy++;
	}

	wr32(REG_FREQUENCY, MHz_72);

	wr8(REG_PCLK, 0); // disable pclk until other registers set
	// wr8(REG_PWM_DUTY, 0); // disable backlight

	// Display register configuration
	/* Timings found via the BT817 datasheet
	 * (https://brtchip.com/wp-content/uploads/2022/04/DS_BT817_8.pdf
	 * page 26) and the timings table provided in the RGB controller
	 * datasheet used by the Crystalfontz display
	 * (https://www.crystalfontz.com/controllers/datasheet-viewer.php?id=486)
	 */

	wr16(REG_HCYCLE, LCD_HCYCLE);
	wr16(REG_HOFFSET, LCD_HOFFSET);
	wr16(REG_HSYNC0, LCD_HSYNC0);
	wr16(REG_HSYNC1, LCD_HSYNC1);
	wr16(REG_VCYCLE, LCD_VCYCLE);
	wr16(REG_VOFFSET, LCD_VOFFSET);
	wr16(REG_VSYNC0, LCD_VSYNC0);
	wr16(REG_VSYNC1, LCD_VSYNC1);
	wr8(REG_SWIZZLE, 0);
	wr8(REG_PCLK_POL, LCD_PCLKPOL);
	/* PCLK Polarity was Found in pin description of Crystalfontz display
	 * datasheet
	 * (https://www.crystalfontz.com/products/document/5145/CFAF800480E3-050SNDatasheet.pdf)*/
	wr8(REG_CSPREAD, 0);
	wr8(REG_DITHER, 0);
	wr16(REG_HSIZE, HPX);
	wr16(REG_VSIZE, VLH);

	/**pin drive stuff */
	PIN_DRIVE_TABLE;

	for (unsigned int i = 0; i < sizeof(Pin_Drive_Table); i++) {
		host_command((0x7000) + (0xFF & Pin_Drive_Table[i]));
	}

	wr16(REG_ADAPTIVE_FRAMERATE, 0);

	// Clear the display to black
	wr32(RAM_DL + 0, CLEAR_COLOR_RGB(0, 0, 0));
	wr32(RAM_DL + 4, CLEAR(1, 1, 1));
	wr32(RAM_DL + 8, DISPLAY());

	// signal to start rendering display list
	wr8(REG_DLSWAP, DLSWAP_FRAME);

	// enable backlight
	// wr32(REG_PWM_DUTY, 128);
	// wr32(REG_PWM_HZ, 250);
	// wr16(REG_GPIOX_DIR, 0xFFFF);
	// wr16(REG_GPIOX, 0xFFFF);
	wr16(REG_GPIOX, rd16(REG_GPIOX) | 0x8000);

	// set pixel clock, which signals BT817 to start rendering
	/** NOTE: PCLK should be between 23-27 MHz */
	// HAL_Delay(500);
	//  wr16(REG_PCLK_FREQ, 0xD14);
	/*Corresponds to a frequency of 25.5 MHz as per Table 4-11 found on
	 * page 25 of the BT817 datasheet */
	// wr8(REG_PCLK, 0x1);
	wr8(REG_PCLK, LCD_PCLK);
	// Backlight frequency default is 250Hz. That is fine for CFA10100
	wr16(REG_PWM_HZ, 250);
	// Crystalfontz EVE displays have soft start. No need to ramp.
	//   EVE_REG_Write_8(EVE_REG_PWM_DUTY,128);
	//~~
	// wr8(REG_PWM_DUTY, 128); //for some reason explicitly setting a PWM
	// duty causes the backlight to not turn on
}
