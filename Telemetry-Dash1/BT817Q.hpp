// BT817Q.hpp
// BT817Q (Bridgetek EVE4 LCD Controller) driver interface
// Written by: Njeri Gachoka

#ifndef BT817Q_HPP
#define BT817Q_HPP

#include "address_mapping.h"
#include "dl_commands.h"
#include "host_commands.h"
#include "mbed.h"
#include "ui.h"

/**
 * @brief  Timing / geometry description for a specific TFT panel.
 *
 * Feed one of these structs to BT817Q's constructor.  All values map 1-to-1
 * onto BT81x timing registers — see the datasheet if you need custom panels.
 */
struct EvePanel {
  uint16_t width;   // active width  (pixels)
  uint16_t height;  // active height (pixels)
  uint16_t hCycle;  // total clocks per line (incl. porch + sync)
  uint16_t hOffset; // back‑porch length (clocks) before active area
  uint16_t hSync0;  // HSYNC start  (clocks)
  uint16_t hSync1;  // HSYNC end    (clocks)  — width = hSync1-hSync0
  uint16_t vCycle;  // total lines per frame
  uint16_t vOffset; // back‑porch lines  before active area
  uint16_t vSync0;  // VSYNC start  (lines)
  uint16_t vSync1;  // VSYNC end    (lines)   — height = vSync1-vSync0
  uint8_t pclk;     // pixel‑clock divisor (0 = off, n = PLL/n)
};

/**
 * @brief Handy pre-defined panel timings.
 *
 * Add your own constexpr blocks here if you spin a custom LCD.
 */
namespace EvePresets {
// CrystalFontz CFA800480E3-050Sx (5-inch IPS, 800 x 480 @ 60 Hz)
inline constexpr EvePanel CFA800480E3{
    800,
    480, // width, height
    928,
    88, // hCycle, hOffset
    0,
    48, // hSync0, hSync1  (48‑pixel pulse)
    525,
    32, // vCycle, vOffset
    0,
    3, // vSync0, vSync1  (3‑line pulse)
    2  // PCLK = 60 MHz / 2 = 30 MHz
};
} // namespace EvePresets

/**
 * @class BT817Q
 * @brief Minimal C++ wrapper around the Bridgetek BT817Q (EVE4) graphics IC.
 */
class BT817Q {
public:
  /// Text‑rendering options (bit field matches BT8xx CMD_TEXT)
  enum TextOpt : uint16_t {
    OPT_CENTER = 1536,  // horizontally & vertically centred
    OPT_RIGHTX = 2048,  // x = right edge of text
    OPT_CENTERY = 1024, // y = centre baseline
    OPT_CENTERX = 512,
    OPT_FORMAT = 4096, // numeric formatting (%d)
    OPT_FILL = 8192
  };

  static constexpr uint16_t OPT_SIGNED = 256;

  enum GaugeOpt : uint16_t {
    OPT_FLAT = 256,
    OPT_NOBACK = 4096,
    OPT_NOTICKS = 8192,
    OPT_NOPOINTER = 16384
  };

  BT817Q(PinName mosi,
         PinName miso,
         PinName sck,
         PinName cs,
         PinName pdn,
         PinName irq,
         EvePanel panel);

  // Public facing API
  void init(const EvePanel &panel);
  void startFrame();                           // CMD_DLSTART + CLEAR
  void clear(uint8_t r, uint8_t g, uint8_t b); // CLEAR_COLOR + CLEAR

  void drawText(int16_t x,
                int16_t y,
                const char *s,
                uint8_t font = 31,
                uint16_t options = OPT_CENTER);

  void drawText(int16_t x,
                int16_t y,
                const char *s,
                Color color,
                uint8_t font = 31,
                uint16_t options = OPT_CENTER);

  void drawFormattedText(int16_t x,
                         int16_t y,
                         const char *s,
                         uint8_t font = 31,
                         uint16_t options = OPT_CENTER,
                         ...);

  void drawFormattedText(int16_t x,
                         int16_t y,
                         const char *s,
                         Color color,
                         uint8_t font = 31,
                         uint16_t options = OPT_CENTER,
                         ...);

  /**
   * Draws a number at (x,y) with optionally a fixed width
   * Unsigned by default, can be made signed by setting OPT_SIGNED
   * @param x position x coordinate
   * @param y position y coordinate
   * @param n number to display
   * @param font font 0-31
   * @param width number 1-9 setting fixed number of digits
   * @param options Text Options or signed
   */
  void drawNumber(int16_t x,
                  int16_t y,
                  uint32_t n,
                  int16_t font = 31,
                  uint8_t width = 0,
                  uint16_t options = OPT_CENTER);

  /**
   * Draws a number at (x, y) with optionally a fixed width, and a specified
   * color Unsigned by default, can be made signed by setting OPT_SIGNED
   *    * @param x position x coordinate
   * @param y position y coordinate
   * @param n number to display
   * @param font font 0-31
   * @param color color of text
   * @param width number 1-9 setting fixed number of digits
   * @param options Text Options or signed
   */
  void drawNumber(int16_t x,
                  int16_t y,
                  uint32_t n,
                  Color color,
                  int16_t font = 31,
                  uint8_t width = 0,
                  uint16_t options = OPT_CENTER);

  void setMainColor(Color color);
  void setBackgroundColor(Color color);
  void setForegroundColor(Color color);

  /**
   *Draws a circular point at (x,y) with specified radius and color
   * Changes main color
   * @param radius radius of point in 1/16px, range 0-8191 (0-511 px)
   * @param color RGB(A) value of point
   */
  void drawPoint(uint16_t radius, uint16_t x, uint16_t y, Color color);

  /**
   * Draws a circular point at (x, y) with specified radius and current main
   * color
   * @param radius radius of point in 1/16px, range 0-8191 (0-511 px)
   */
  void drawPoint(uint16_t radius, uint16_t x, uint16_t y);

  /**
   * Draw a rectangle with the top left and bottom right corners at specified
   * points
   * @param corner_radius rounded radius around corners, in 16ths of a pixel
   */
  void
  drawRect(Point top_left, Point bottom_right, uint16_t corner_radius = 16);

  /**
   * Draw a rectangle with the top left and bottom right corners at specified
   * points
   * @param corner_radius rounded radius around corners, in 16ths of a pixel
   */
  void drawRect(Point top_left,
                Point bottom_right,
                Color color,
                uint16_t corner_radius = 16);

  /**
   * Draws a line from (start.x, start.y) to (end.x, end.y)
   * @param start - start coordinate
   * @param end - end coordinate
   * @param width - width, in 16ths of a pixel (default = 16 = 1px)
   */
  void drawLine(Point start, Point end, uint16_t width = 16);

  /**
   * Draws a line from (start.x, start.y) to (end.x, end.y)
   * @param start - start coordinate
   * @param end - end coordinate
   * @param width - width, in 16ths of a pixel (default = 16 = 1px)
   * @param color - Color of line
   */
  void drawLine(Point start, Point end, Color color, uint16_t width = 16);

  /**
   *   * @param pos Point of gauge center (in pixels)
   * @param r radius, in pixels
   * @param val value pointed to, in range [0,range]
   * @param range max value of gauge
   * @param major number of major divisions
   * @param minor number of minor divisions per major division
   * @param options OPT_FLAT, OPT_NOTICKS, OPT_NOBACK, OPT_NOPOINTER, default
   * OPT_FLAT
   */
  void drawGauge(Point pos,
                 int16_t r,
                 uint16_t val,
                 uint16_t range,
                 uint16_t major = 10,
                 uint16_t minor = 10,
                 uint16_t options = OPT_FLAT);

  /**
   * A gauge with set colors for the background and foreground
   * @param pos Point of gauge center (in pixels)
   * @param r radius, in pixels
   * @param val value pointed to, in range [0,range]
   * @param range max value of gauge
   * @param major number of major divisions
   * @param minor number of minor divisions per major division
   * @param options OPT_FLAT, OPT_NOTICKS, OPT_NOBACK, OPT_NOPOINTER, default
   * OPT_FLAT
   * @param bg color of background of gauge
   * @param fg color of needle and ticks
   */
  void drawGauge(Point pos,
                 int16_t r,
                 uint16_t val,
                 uint16_t range,
                 Color bg,
                 Color fg = Color{255, 255, 255},
                 uint16_t major = 10,
                 uint16_t minor = 10,
                 uint16_t options = OPT_FLAT);

  /**
   * A gauge with set colors for the background and foreground
   * @param pos Point of gauge center (in pixels)
   * @param r radius, in pixels
   * @param val value pointed to, in range [0,range]
   * @param range max value of gauge
   * @param major number of major divisions
   * @param minor number of minor divisions per major division
   * @param options OPT_FLAT, OPT_NOTICKS, OPT_NOBACK, OPT_NOPOINTER, default
   * OPT_FLAT
   * @param bg color of background of gauge
   * @param majorTick color of the major division tick marks
   * @param minorTick color of the minor division tick marks
   * @param needle color of the gauge needle
   */
  void drawGauge(Point pos,
                 int16_t r,
                 uint16_t val,
                 uint16_t range,
                 Color bg,
                 Color majorTick,
                 Color minorTick,
                 Color needle,
                 uint16_t major = 10,
                 uint16_t minor = 10,
                 uint16_t options = OPT_FLAT);

  void drawProgressBar(Point pos,
                       int16_t w,
                       int16_t h,
                       uint16_t val,
                       uint16_t range,
                       bool flat = true);

  void drawProgressBar(Point pos,
                       int16_t w,
                       int16_t h,
                       uint16_t val,
                       uint16_t range,
                       Color bg,
                       Color bar = WHITE,
                       bool flat = true);

  void endFrame(); // DISPLAY + CMD_SWAP
  void setBacklight(bool on);

private:
  // Low‑level internal helpers
  void hostCmd(uint8_t cmd, uint8_t param);
  void selectWriteAddress(uint32_t addr);
  void selectReadAddress(uint32_t addr);
  void write8(uint32_t addr, uint8_t data);
  void write16(uint32_t addr, uint16_t data);
  void write32(uint32_t addr, uint32_t data);
  uint8_t read8(uint32_t addr);
  uint16_t read16(uint32_t addr);
  uint32_t read32(uint32_t addr);
  void cmd(uint32_t word);
  void cmdString(const char *s);
  void cmdWait();

  // Internal data
  SPI _spi;
  DigitalOut _cs;
  DigitalOut _pdn;
  DigitalIn _irq;

  EvePanel _p;

  // write‑pointer into RAM_CMD (address offset)
  uint16_t _cmd_wp = 0;

  uint8_t _current_alpha = 255;

  // Display list RAM
  // static constexpr uint32_t RAM_DL        = 0x300000UL;  // end is 0x301FFFUL
  // Register base addresses (BT817Q datasheet, Table 5‑2)
  // static constexpr uint32_t RAM_CMD       = 0x308000UL;  // end is 0x308FFFUL
  // static constexpr uint32_t REG_CMD_READ  = 0x3020F8UL;
  // static constexpr uint32_t REG_CMD_WRITE = 0x3020FCUL;
  // static constexpr uint32_t REG_HSIZE     = 0x302034UL;
  // static constexpr uint32_t REG_HCYCLE    = 0x30202CUL;
  // static constexpr uint32_t REG_HOFFSET   = 0x302030UL;
  // static constexpr uint32_t REG_HSYNC0    = 0x302038UL;
  // static constexpr uint32_t REG_HSYNC1    = 0x30203CUL;
  // static constexpr uint32_t REG_VSIZE     = 0x302048UL;
  // static constexpr uint32_t REG_VCYCLE    = 0x302040UL;
  // static constexpr uint32_t REG_VOFFSET   = 0x302044UL;
  // static constexpr uint32_t REG_VSYNC0    = 0x30204CUL;
  // static constexpr uint32_t REG_VSYNC1    = 0x302050UL;
  // static constexpr uint32_t REG_SWIZZLE   = 0x302064UL;
  // static constexpr uint32_t REG_PCLK_POL  = 0x30206CUL;
  // static constexpr uint32_t REG_CSPREAD   = 0x302068UL;
  // static constexpr uint32_t REG_DITHER    = 0x302060UL;
  // static constexpr uint32_t REG_GPIO_DIR  = 0x302090UL;
  // static constexpr uint32_t REG_GPIO      = 0x302094UL;
  // static constexpr uint32_t REG_PCLK      = 0x302070UL;
  // static constexpr uint32_t REG_ID        = 0x302000UL;
  // static constexpr uint32_t REG_CPURESET  = 0x302020UL;
  // static constexpr uint32_t REG_FREQUENCY = 0x30200CUL;
  // static constexpr uint32_t REG_DLSWAP    = 0x302054UL;
  // static constexpr uint32_t REG_PWM_DUTY  = 0x3020D4UL;
  // static constexpr uint32_t REG_PWM_HZ    = 0x3020D0UL;
  // static constexpr uint32_t REG_GPIOX     = 0x30209CUL;
  // We aren't utilizing the audio_pwm pin on the ribbon cable
  // static constexpr uint32_t REG_VOL_SOUND = 0x302084UL;
  // static constexpr uint32_t REG_SOUND     = 0x302088UL;
  // static constexpr uint32_t REG_PLAY      = 0x30208CUL;

  // Host commands (BT817Q datasheet, Section 4.1.5)
  // static constexpr uint8_t  HCMD_ACTIVE   = 0x00;
  // static constexpr uint8_t  HCMD_STANDBY  = 0x41;
  // static constexpr uint8_t  HCMD_SLEEP    = 0x42;
  //
  // static constexpr uint8_t HCMD_CLKEXT = 0x44;
  // static constexpr uint8_t HCMD_CLKSEL = 0x61;

  // Co‑processor op‑codes
  static constexpr uint32_t CMD_DLSTART = 0xFFFFFF00UL;
  static constexpr uint32_t CMD_SWAP = 0xFFFFFF01UL;
  static constexpr uint32_t CMD_TEXT = 0xFFFFFF0CUL;
  static constexpr uint32_t CMD_NUMBER = 0xFFFFFF2E;
  static constexpr uint32_t CMD_LOGO = 0xFFFFFF31UL;
  static constexpr uint32_t CMD_FGCOLOR = 0xFFFFFF0A;
  static constexpr uint32_t CMD_BGCOLOR = 0xFFFFFF09;
  static constexpr uint32_t CMD_GAUGE = 0xFFFFFF13;

  static constexpr uint32_t CMD_PROGRESS = 0xFFFFFF0F;

  // Display‑list macros (see BT81x Programming Guide)
  static constexpr uint32_t DISPLAY = 0x00000000UL;
  // static constexpr uint32_t CLEAR_COLOR_RGB(uint8_t r, uint8_t g, uint8_t b)
  // {
  //     return 0x02000000UL | (uint32_t(r) << 16) | (uint32_t(g) << 8) | b;
  // }
  // static constexpr uint32_t CLEAR(uint8_t c, uint8_t s, uint8_t t) {
  //     return 0x26000000UL | ((c & 1) << 2) | ((s & 1) << 1) | (t & 1);
  // }
  // static constexpr uint32_t SCISSOR_SIZE(uint) {
  //     return
  // }
};

#endif // BT817Q_HPP
