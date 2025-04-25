// BT817Q.hpp
// BT817Q (Bridgetek EVE4 LCD Controller) driver interface
// Written by: Njeri Gachoka

#ifndef BT817Q_HPP
#define BT817Q_HPP

#include "mbed.h"
using namespace chrono_literals;

/**
 * @brief  Timing / geometry description for a specific TFT panel.
 *
 * Feed one of these structs to BT817Q::init().  All values map 1-to-1 onto
 * BT81x timing registers — see the datasheet if you need custom panels.
 */
struct EvePanel {
    uint16_t width;      // active width  (pixels)
    uint16_t height;     // active height (pixels)
    uint16_t hCycle;     // total clocks per line (incl. porch + sync)
    uint16_t hOffset;    // back‑porch length (clocks) before active area
    uint16_t hSync0;     // HSYNC start  (clocks)
    uint16_t hSync1;     // HSYNC end    (clocks)  — width = hSync1-hSync0
    uint16_t vCycle;     // total lines per frame
    uint16_t vOffset;    // back‑porch lines  before active area
    uint16_t vSync0;     // VSYNC start  (lines)
    uint16_t vSync1;     // VSYNC end    (lines)   — height = vSync1-vSync0
    uint8_t  pclk;       // pixel‑clock divisor (0 = off, n = PLL/n)
};

/**
 * @brief Handy pre-defined panel timings.
 *
 * Add your own constexpr blocks here if you spin a custom LCD.
 */
namespace EvePresets {
    // CrystalFontz CFA800480E3-050Sx (5-inch IPS, 800 x 480 @ 60 Hz)
    inline constexpr EvePanel CFA800480E3 {
        800, 480,   // width, height
        928, 88,    // hCycle, hOffset
        0, 48,      // hSync0, hSync1  (48‑pixel pulse)
        525, 32,    // vCycle, vOffset
        0, 3,       // vSync0, vSync1  (3‑line pulse)
        2           // PCLK = 60 MHz / 2 = 30 MHz
    };
}

/**
 * @class BT817Q
 * @brief Minimal C++ wrapper around the Bridgetek BT817Q (EVE4) graphics IC.
 */
class BT817Q {
public:
    // Types -------------------------------------------------------------------

    /// Text‑rendering options (bit field matches BT8xx CMD_TEXT)
    enum TextOpt : uint16_t {
        OPT_CENTER       = 0x0000,    // horizontally & vertically centred
        OPT_RIGHTX       = 0x0001,    // x = right edge of text
        OPT_CENTERY      = 0x0002,    // y = centre baseline
        OPT_RIGHT        = 0x0006,    // right‑aligned
        OPT_FORMAT       = 0x0400,    // numeric formatting (%d)
        OPT_MEDIAFIFO    = 0x0800
    };

    // Constructor -------------------------------------------------------------
    
    BT817Q(PinName mosi, PinName miso, PinName sck,
           PinName cs,   PinName pdn, PinName irq = NC);

    // Top‑level API -----------------------------------------------------------
    
    void init(const EvePanel &panel);
    void startFrame();                           // CMD_DLSTART + CLEAR
    void clear(uint8_t r, uint8_t g, uint8_t b); // CLEAR_COLOR + CLEAR
    void drawText(int16_t x, int16_t y,
                  uint8_t font, const char *s,
                  uint16_t options = OPT_CENTER);
    void endFrame();                             // DISPLAY + CMD_SWAP
    void setBacklight(bool on);

private:
    // Low‑level helpers -------------------------------------------------------
    
    void hostCmd(uint8_t cmd);
    void write8(uint32_t addr, uint8_t data);
    void write16(uint32_t addr, uint16_t data);
    void write32(uint32_t addr, uint32_t data);
    uint8_t read8(uint32_t addr);
    uint16_t read16(uint32_t addr);
    uint32_t read32(uint32_t addr);
    void cmd(uint32_t word);
    void cmdString(const char *s);
    void cmdWait();

    // Data --------------------------------------------------------------------
   
    SPI _spi;
    DigitalOut _cs;
    DigitalOut _pdn;
    DigitalIn _irq;
    uint16_t _cmd_wp = 0;                       // write‑pointer into RAM_CMD
    
    // Memory map
    static constexpr uint32_t RAM_DL        = 0x300000UL;  // end is 0x301FFFUL

    // Register base addresses (BT817Q datasheet, Table 5‑2)
    static constexpr uint32_t RAM_CMD       = 0x308000UL;  // end is 0x308FFFUL
    static constexpr uint32_t REG_CMD_READ  = 0x3020F8UL;
    static constexpr uint32_t REG_CMD_WRITE = 0x3020FCUL;
    static constexpr uint32_t REG_HSIZE     = 0x302034UL;
    static constexpr uint32_t REG_HCYCLE    = 0x30202CUL;
    static constexpr uint32_t REG_HOFFSET   = 0x302030UL;
    static constexpr uint32_t REG_HSYNC0    = 0x302038UL;
    static constexpr uint32_t REG_HSYNC1    = 0x30203CUL;
    static constexpr uint32_t REG_VSIZE     = 0x302048UL;
    static constexpr uint32_t REG_VCYCLE    = 0x302040UL;
    static constexpr uint32_t REG_VOFFSET   = 0x302044UL;
    static constexpr uint32_t REG_VSYNC0    = 0x30204CUL;
    static constexpr uint32_t REG_VSYNC1    = 0x302050UL;
    static constexpr uint32_t REG_SWIZZLE   = 0x302064UL;
    static constexpr uint32_t REG_PCLK_POL  = 0x30206CUL;
    static constexpr uint32_t REG_CSPREAD   = 0x302068UL;
    static constexpr uint32_t REG_DITHER    = 0x302060UL;
    static constexpr uint32_t REG_GPIO_DIR  = 0x302090UL;
    static constexpr uint32_t REG_GPIO      = 0x302094UL;
    static constexpr uint32_t REG_PCLK      = 0x302070UL;
    static constexpr uint32_t REG_ID        = 0x302000UL;
    static constexpr uint32_t REG_CPURESET  = 0x302020UL;
    static constexpr uint32_t REG_FREQUENCY = 0x30200CUL;
    static constexpr uint32_t REG_DLSWAP    = 0x302054UL;

    // Host commands (BT817Q datasheet, Section 4.1.5)
    static constexpr uint8_t  HCMD_ACTIVE   = 0x00;
    static constexpr uint8_t  HCMD_STANDBY  = 0x41;
    static constexpr uint8_t  HCMD_SLEEP    = 0x42;

    // Co‑processor op‑codes
    static constexpr uint32_t CMD_DLSTART   = 0xFFFFFF00UL;
    static constexpr uint32_t CMD_SWAP      = 0xFFFFFF01UL;
    static constexpr uint32_t CMD_TEXT      = 0xFFFFFF0CUL;

    // Display‑list macros (see BT81x Programming Guide)
    static constexpr uint32_t DISPLAY       = 0x00000000UL;
    static constexpr uint32_t CLEAR_COLOR_RGB(uint8_t r, uint8_t g, uint8_t b) {
        return 0x02000000UL | (uint32_t(r) << 16) | (uint32_t(b) << 8) | g;
    }
    static constexpr uint32_t CLEAR(uint8_t c, uint8_t s, uint8_t t) {
        return 0x26000000UL | ((c & 1) << 2) | ((s & 1) << 1) | (t & 1);
    }
};

#endif // BT817Q_HPP