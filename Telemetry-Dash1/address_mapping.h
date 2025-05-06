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

#define RAM_G                  0x000000UL
#define ROM_FONT               0x1E0000UL
#define ROM_FONTROOT           0x2FFFFCUL
#define RAM_DL                 0x300000UL
#define RAM_REG                0x302000UL
#define RAM_CMD                0x308000UL
#define RAM_ERR_REPORT         0x309800UL

/*
 * The following are addresses of the registers described in Section 3 of the
 * programming guide. They consist of an offset added to the RAM_REG address
 * defined above. These following constants ALREADY INCLUDE the ram_reg address
 * added to the offset, so you DO NOT need to add them to RAM_REG when using
 * them
 */

// Graphics
#define REG_TAG                RAM_REG + 0x7cUL
#define REG_TAG_Y              RAM_REG + 0x78UL
#define REG_TAG_X              RAM_REG + 0x74UL
#define REG_PCLK               RAM_REG + 0x70UL
#define REG_PCLK_POL           RAM_REG + 0x6cUL
#define REG_CSPREAD            RAM_REG + 0x68UL
#define REG_SWIZZLE            RAM_REG + 0x64UL
#define REG_DITHER             RAM_REG + 0x60UL
#define REG_OUTBITS            RAM_REG + 0x5cUL
#define REG_ROTATE             RAM_REG + 0x58UL
#define REG_DLSWAP             RAM_REG + 0x54UL
#define REG_VSYNC1             RAM_REG + 0x50UL
#define REG_VSYNC0             RAM_REG + 0x4CUL
#define REG_VSIZE              RAM_REG + 0x48UL
#define REG_VOFFSET            RAM_REG + 0x44UL
#define REG_VCYCLE             RAM_REG + 0x40UL
#define REG_HSYNC1             RAM_REG + 0x3cUL
#define REG_HSYNC0             RAM_REG + 0x38UL
#define REG_HSIZE              RAM_REG + 0x34UL
#define REG_HOFFSET            RAM_REG + 0x30UL
#define REG_HCYCLE             RAM_REG + 0x2CUL
// sound
#define REG_PLAY               RAM_REG + 0x8cUL
#define REG_SOUND              RAM_REG + 0x88UL
#define REG_VOL_SOUND          RAM_REG + 0x84UL
#define REG_VOL_PB             RAM_REG + 0x84UL
#define REG_PLAYBACK_PLAY      RAM_REG + 0xCCUL
#define REG_PLAYBACK_LOOP      RAM_REG + 0xC8UL
#define REG_PLAYBACK_FORMAT    RAM_REG + 0xC4UL
#define REG_PLAYBACK_FREQ      RAM_REG + 0xc0UL
#define REG_PLAYBACK_READPTR   RAM_REG + 0xBCUL
#define REG_PLAYBACK_LENGTH    RAM_REG + 0xB8UL
#define REG_PLAYBACK_START     RAM_REG + 0xB4UL
#define REG_PLAYBACK_PAUSE     RAM_REG + 0x5ecUL
// flash
#define REG_FLASH_STATUS       RAM_REG + 0x5f0UL
#define REG_FLASH_SIZE         RAM_REG + 0x7024UL
// touch
// not gonna bother defining the touch registers because there's a lot of them
// and the pcb isn't even wired for touch coprocessor
#define REG_CMD_DL             RAM_REG + 0x100UL
#define REG_CMD_WRITE          RAM_REG + 0xFCUL
#define REG_CMD_READ           RAM_REG + 0xF8UL
#define REG_CMDB_SPACE         RAM_REG + 0x574UL
#define REG_CMDB_WRITE         RAM_REG + 0x578UL
// misc
#define REG_CPURESET           RAM_REG + 0x20UL
#define REG_MACRO_1            RAM_REG + 0xDCUL
#define REG_MACRO_0            RAM_REG + 0xD8UL
#define REG_PWM_DUTY           RAM_REG + 0xD4UL
#define REG_PWM_HZ             RAM_REG + 0xD0UL
#define REG_INT_MASK           RAM_REG + 0xB0UL
#define REG_INT_EN             RAM_REG + 0xACUL
#define REG_INT_FLAGS          RAM_REG + 0xA8UL
#define REG_GPIO_DIR           RAM_REG + 0x90UL
#define REG_GPIO               RAM_REG + 0x94UL
#define REG_GPIOX_DIR          RAM_REG + 0x98UL
#define REG_GPIOX              RAM_REG + 0x9CUL
#define REG_FREQUENCY          RAM_REG + 0xCUL
#define REG_CLOCK              RAM_REG + 0x8UL
#define REG_FRAMES             RAM_REG + 0x4UL
#define REG_ID                 RAM_REG + 0x0UL
#define REG_SPI_WIDTH          RAM_REG + 0x180UL
#define REG_ADAPTIVE_FRAMERATE RAM_REG + 0x57CUL
#define REG_UNDERRUN           RAM_REG + 0x60cUL
#define REG_AH_HCYCLE_MAX      RAM_REG + 0x610UL
#define REG_PCLK_FREQ          RAM_REG + 0x614UL
#define REG_PCLK_2x            RAM_REG + 0x618UL
// special
#define REG_TRACKER            RAM_REG + 0x7000UL
#define REG_TRACKER_1          RAM_REG + 0x7004UL
#define REG_TRACKER_2          RAM_REG + 0x7008UL
#define REG_TRACKER_3          RAM_REG + 0x700CUL
#define REG_TRACKER_4          RAM_REG + 0x7010UL
#define REG_MEDIAFIFO_READ     RAM_REG + 0x7014UL
#define REG_MEDIAFIFO_WRITE    RAM_REG + 0x7018UL
#define REG_PLAY_CONTROL       RAM_REG + 0x714EUL
#define REG_ANIM_ACTIVE        RAM_REG + 0x702CUL
#define REG_COPRO_PATCH_PTR    RAM_REG + 0x7162UL

#endif
