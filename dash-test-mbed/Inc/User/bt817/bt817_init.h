#ifndef __BT817_INIT_H
#define __BT817_INIT_H

void bt817_init(void);

// Horizontal timing (minimum values from ILI6122_SPEC_V008.pdf page 45)
// Target 60Hz frame rate, using the largest possible line time in order to
// maximize the time that the EVE has to process each line.
#define HPX                         (800) // Horizontal Pixel Width
#define HSW                         (4)   // Horizontal Sync Width (1~40)
#define HBP                         (8) // Horizontal Back Porch (must be 46, includes HSW)
#define HFP                         (8) // Horizontal Front Porch (16~210~354)
#define HPP                         (178) // Horizontal Pixel Padding (tot=863: 862~1056~1200)
// EVE needs at least 1 here
// Define the constants needed by the EVE based on the timing
// Active width of LCD display
#define LCD_WIDTH                   (HPX)
// Start of horizontal sync pulse
#define LCD_HSYNC0                  (HFP)
// End of horizontal sync pulse
#define LCD_HSYNC1                  (HFP + HSW)
// Start of active line
#define LCD_HOFFSET                 (HFP + HSW + HBP)
// Total number of clocks per line
#define LCD_HCYCLE                  (HPX + HFP + HSW + HBP + HPP)
//----------------------------------------------------------------------------
// Vertical timing (minimum values from ILI6122_SPEC_V008.pdf page 46)
#define VLH                         (480) // Vertical Line Height
#define VS                          (4)   // Vertical Sync (in lines)  (1~20)
#define VBP                         (8) // Vertical Back Porch (must be 23, includes VS)
#define VFP                         (8) // Vertical Front Porch (7~22~147)
#define VLP                         (1) // Vertical Line Padding (tot=511: 510~525~650)
// EVE needs at least 1 here
// Define the constants needed by the EVE based on the timing
// Active height of LCD display
#define LCD_HEIGHT                  (VLH)
// Start of vertical sync pulse
#define LCD_VSYNC0                  (VFP)
// End of vertical sync pulse
#define LCD_VSYNC1                  (VFP + VS)
// Start of active screen
#define LCD_VOFFSET                 (VFP + VS + VBP)
// Total number of lines per screen
#define LCD_VCYCLE                  (VLH + VFP + VS + VBP + VLP)

// Pixel clock divisor (based on 72MHz internal clock)
//   0 = disable
//   1 = 60MHz (?)
//   2 = 36MHz
//   3 = 24MHz
//   4 = 18MHz
//   5 = 14.4MHz
//   6 = 12MHz
//   7 = 10.3MHz
//   8 = 9MHz
//   9 = 8MHz
//   10 = 7.2MHz
//   etc
// Our target is 33MHz.
// LCD_PCLK divisor of 2 gives us 36MHz
#define LCD_PCLK                    (2)

// Define active edge of PCLK. Observed by scope:
//  0: Data is put out coincident with falling edge of the clock.
//     Rising edge of the clock is in the middle of the data.
//  1: Data is put out coincident with rising edge of the clock.
//     Falling edge of the clock is in the middle of the data.
#define LCD_PCLKPOL                 (1)

#define EVE_PINDRIVE_HR_5mA         (0x00)
#define EVE_PINDRIVE_HR_10mA        (0x01)
#define EVE_PINDRIVE_HR_15mA        (0x02)
#define EVE_PINDRIVE_HR_20mA        (0x03)
#define EVE_PINDRIVE_LR_1p2mA       (0x00)
#define EVE_PINDRIVE_LR_2p4mA       (0x01)
#define EVE_PINDRIVE_LR_3p6mA       (0x02)
#define EVE_PINDRIVE_LR_4p8mA       (0x03)
// Byte 2, bits 7:2 specifies the pin(s)
#define EVE_PINDRIVE_GPIO_0_HR      (0x00) // 0x00 << 2 =  0 * 4 =   0
#define EVE_PINDRIVE_GPIO_1_HR      (0x04) // 0x01 << 2 =  1 * 4 =   4
#define EVE_PINDRIVE_GPIO_2_HR      (0x08) // 0x02 << 2 =  2 * 4 =   8
#define EVE_PINDRIVE_GPIO_3_HR      (0x0C) // 0x03 << 2 =  3 * 4 =  12
#define EVE_PINDRIVE_DISP_LR        (0x20) // 0x08 << 2 =  8 * 4 =  32
#define EVE_PINDRIVE_DE_LR          (0x24) // 0x09 << 2 =  9 * 4 =  36
#define EVE_PINDRIVE_VSYNC_HSYNC_LR (0x28) // 0x0A << 2 = 10 * 4 =  40
#define EVE_PINDRIVE_PCLK_LR        (0x2C) // 0x0B << 2 = 11 * 4 =  44
#define EVE_PINDRIVE_BACKLIGHT_LR   (0x30) // 0x0C << 2 = 12 * 4 =  48
#define EVE_PINDRIVE_RGB_LR         (0x34) // 0x0D << 2 = 13 * 4 =  52
#define EVE_PINDRIVE_AUDIO_L_HR     (0x38) // 0x0E << 2 = 14 * 4 =  56
#define EVE_PINDRIVE_INT_N_HR       (0x3C) // 0x0F << 2 = 15 * 4 =  60
#define EVE_PINDRIVE_CTP_RST_N_HR   (0x40) // 0x10 << 2 = 16 * 4 =  64
#define EVE_PINDRIVE_CTP_SCL_HR     (0x44) // 0x11 << 2 = 17 * 4 =  68
#define EVE_PINDRIVE_CTP_SDA_HR     (0x48) // 0x12 << 2 = 18 * 4 =  72
#define EVE_PINDRIVE_SPI_DATA_HR    (0x4C) // 0x13 << 2 = 19 * 4 =  76
#define EVE_PINDRIVE_SPIM_SCLK_HR   (0x50) // 0x14 << 2 = 20 * 4 =  80
#define EVE_PINDRIVE_SPIM_SS_N_HR   (0x54) // 0x15 << 2 = 21 * 4 =  84
#define EVE_PINDRIVE_SPIM_MISO_HR   (0x58) // 0x16 << 2 = 22 * 4 =  88
#define EVE_PINDRIVE_SPIM_MOSI_HR   (0x5C) // 0x17 << 2 = 23 * 4 =  92
#define EVE_PINDRIVE_SPIM_IO2_HR    (0x60) // 0x18 << 2 = 24 * 4 =  96
#define EVE_PINDRIVE_SPIM_IO3_HR    (0x64) // 0x19 << 2 = 25 * 4 = 100

#define PIN_DRIVE_TABLE                                                        \
	const uint8_t Pin_Drive_Table[22] = {                                  \
	    (EVE_PINDRIVE_GPIO_0_HR | EVE_PINDRIVE_HR_5mA),                    \
	    (EVE_PINDRIVE_GPIO_1_HR | EVE_PINDRIVE_HR_5mA),                    \
	    (EVE_PINDRIVE_GPIO_2_HR | EVE_PINDRIVE_HR_5mA),                    \
	    (EVE_PINDRIVE_GPIO_3_HR | EVE_PINDRIVE_HR_5mA),                    \
	    (EVE_PINDRIVE_DISP_LR | EVE_PINDRIVE_LR_1p2mA),                    \
	    (EVE_PINDRIVE_DE_LR | EVE_PINDRIVE_LR_1p2mA),                      \
	    (EVE_PINDRIVE_VSYNC_HSYNC_LR | EVE_PINDRIVE_LR_1p2mA),             \
	    (EVE_PINDRIVE_PCLK_LR | EVE_PINDRIVE_LR_3p6mA),                    \
	    (EVE_PINDRIVE_BACKLIGHT_LR | EVE_PINDRIVE_LR_1p2mA),               \
	    (EVE_PINDRIVE_RGB_LR | EVE_PINDRIVE_LR_1p2mA),                     \
	    (EVE_PINDRIVE_AUDIO_L_HR | EVE_PINDRIVE_HR_5mA),                   \
	    (EVE_PINDRIVE_INT_N_HR | EVE_PINDRIVE_HR_5mA),                     \
	    (EVE_PINDRIVE_CTP_RST_N_HR | EVE_PINDRIVE_HR_5mA),                 \
	    (EVE_PINDRIVE_CTP_SCL_HR | EVE_PINDRIVE_HR_5mA),                   \
	    (EVE_PINDRIVE_CTP_SDA_HR | EVE_PINDRIVE_HR_5mA),                   \
	    (EVE_PINDRIVE_SPI_DATA_HR | EVE_PINDRIVE_HR_5mA),                  \
	    (EVE_PINDRIVE_SPIM_SCLK_HR | EVE_PINDRIVE_HR_10mA),                \
	    (EVE_PINDRIVE_SPIM_SS_N_HR | EVE_PINDRIVE_HR_5mA),                 \
	    (EVE_PINDRIVE_SPIM_MISO_HR | EVE_PINDRIVE_HR_5mA),                 \
	    (EVE_PINDRIVE_SPIM_MOSI_HR | EVE_PINDRIVE_HR_5mA),                 \
	    (EVE_PINDRIVE_SPIM_IO2_HR | EVE_PINDRIVE_HR_5mA),                  \
	    (EVE_PINDRIVE_SPIM_IO3_HR | EVE_PINDRIVE_HR_5mA)}

#endif