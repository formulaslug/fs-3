/* All of the definitions in this file are based on the information provided in
 * the BT81x series Programming Guide. The programming guide can be found at:
 * https://www.mouser.com/catalog/additional/BRT_AN_033_BT81XSeriesProgrammingGuidev2.0.pdf
 */

/*
 * Addresses of different sections of BT817 internal memory
 * More details can be found in Section 2.1 "Address Space" of the BT81x
 * Programming Guide
 */
#ifndef ADDRESS_MAPPING_H
#define ADDRESS_MAPPING_H

#define RAM_G                  0x000000
#define ROM_FONT               0x1E0000
#define ROM_FONTROOT           0x2FFFFC
#define RAM_DL                 0x300000
#define RAM_REG                0x302000
#define RAM_CMD                0x308000
#define RAM_ERR_REPORT         0x309800

/*
 * The following are addresses of the registers described in Section 3 of the
 * programming guide. They consist of an offset added to the RAM_REG address
 * defined above. These following constants ALREADY INCLUDE the ram_reg address
 * added to the offset, so you DO NOT need to add them to RAM_REG when using
 * them
 */

// Graphics
#define REG_TAG                RAM_REG + 0x7c
#define REG_TAG_Y              RAM_REG + 0x78
#define REG_TAG_X              RAM_REG + 0x74
#define REG_PCLK               RAM_REG + 0x70
#define REG_PCLK_POL           RAM_REG + 0x6c
#define REG_CSPREAD            RAM_REG + 0x68
#define REG_SWIZZLE            RAM_REG + 0x64
#define REG_DITHER             RAM_REG + 0x60
#define REG_OUTBITS            RAM_REG + 0x5c
#define REG_ROTATE             RAM_REG + 0x58
#define REG_DLSWAP             RAM_REG + 0x54
#define REG_VSYNC1             RAM_REG + 0x50
#define REG_VSYNC0             RAM_REG + 0x4C
#define REG_VSIZE              RAM_REG + 0x48
#define REG_VOFFSET            RAM_REG + 0x44
#define REG_VCYCLE             RAM_REG + 0x40
#define REG_HSYNC1             RAM_REG + 0x3c
#define REG_HSYNC0             RAM_REG + 0x38
#define REG_HSIZE              RAM_REG + 0x34
#define REG_HOFFSET            RAM_REG + 0x30
#define REG_HCYCLE             RAM_REG + 0x2C
// sound
#define REG_PLAY               RAM_REG + 0x8c
#define REG_SOUND              RAM_REG + 0x88
#define REG_VOL_SOUND          RAM_REG + 0x84
#define REG_VOL_PB             RAM_REG + 0x84
#define REG_PLAYBACK_PLAY      RAM_REG + 0xCC
#define REG_PLAYBACK_LOOP      RAM_REG + 0xC8
#define REG_PLAYBACK_FORMAT    RAM_REG + 0xC4
#define REG_PLAYBACK_FREQ      RAM_REG + 0xc0
#define REG_PLAYBACK_READPTR   RAM_REG + 0xBC
#define REG_PLAYBACK_LENGTH    RAM_REG + 0xB8
#define REG_PLAYBACK_START     RAM_REG + 0xB4
#define REG_PLAYBACK_PAUSE     RAM_REG + 0x5ec
// flash
#define REG_FLASH_STATUS       RAM_REG + 0x5f0
#define REG_FLASH_SIZE         RAM_REG + 0x7024
// touch
// not gonna bother defining the touch registers because there's a lot of them
// and the pcb isn't even wired for touch coprocessor
#define REG_CMD_DL             RAM_REG + 0x100
#define REG_CMD_WRITE          RAM_REG + 0xFC
#define REG_CMD_READ           RAM_REG + 0xF8
#define REG_CMDB_SPACE         RAM_REG + 0x574
#define REG_CMDB_WRITE         RAM_REG + 0x578
// misc
#define REG_CPURESET           RAM_REG + 0x20
#define REG_MACRO_1            RAM_REG + 0xDC
#define REG_MACRO_0            RAM_REG + 0xD8
#define REG_PWM_DUTY           RAM_REG + 0xD4
#define REG_PWM_HZ             RAM_REG + 0xD0
#define REG_INT_MASK           RAM_REG + 0xB0
#define REG_INT_EN             RAM_REG + 0xAC
#define REG_INT_FLAGS          RAM_REG + 0xA8
#define REG_GPIO_DIR           RAM_REG + 0x90
#define REG_GPIO               RAM_REG + 0x94
#define REG_GPIOX_DIR          RAM_REG + 0x98
#define REG_GPIOX              RAM_REG + 0x9C
#define REG_FREQUENCY          RAM_REG + 0xC
#define REG_CLOCK              RAM_REG + 0x8
#define REG_FRAMES             RAM_REG + 0x4
#define REG_ID                 RAM_REG + 0x0
#define REG_SPI_WIDTH          RAM_REG + 0x180
#define REG_ADAPTIVE_FRAMERATE RAM_REG + 0x57C
#define REG_UNDERRUN           RAM_REG + 0x60c
#define REG_AH_HCYCLE_MAX      RAM_REG + 0x610
#define REG_PCLK_FREQ          RAM_REG + 0x614
#define REG_PCLK_2x            RAM_REG + 0x618
// special
#define REG_TRACKER            RAM_REG + 0x7000
#define REG_TRACKER_1          RAM_REG + 0x7004
#define REG_TRACKER_2          RAM_REG + 0x7008
#define REG_TRACKER_3          RAM_REG + 0x700C
#define REG_TRACKER_4          RAM_REG + 0x7010
#define REG_MEDIAFIFO_READ     RAM_REG + 0x7014
#define REG_MEDIAFIFO_WRITE    RAM_REG + 0x7018
#define REG_PLAY_CONTROL       RAM_REG + 0x714E
#define REG_ANIM_ACTIVE        RAM_REG + 0x702C
#define REG_COPRO_PATCH_PTR    RAM_REG + 0x7162

#endif
