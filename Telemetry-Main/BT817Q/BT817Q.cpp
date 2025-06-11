#include "BT817Q.hpp"

BT817Q::BT817Q(PinName mosi,
               PinName miso,
               PinName sck,
               PinName cs,
               PinName pdn,
               PinName irq,
               EvePanel panel)
    : _spi(mosi, miso, sck), _cs(cs), _pdn(pdn), _irq(irq), _p(panel) {
  _pdn=0;
}

void BT817Q::init(const EvePanel &p) {
  printf("Initializing EVE...\n");
  // ThisThread::sleep_for(1000ms);

  ThisThread::sleep_for(20ms);
  _pdn = 0;
  ThisThread::sleep_for(6ms);
  _pdn = 1;
  ThisThread::sleep_for(300ms);

  hostCmd(HCMD_RST_PULSE, 0x00);

  // SPI safe‑start (<= 11 MHz until PCLK up)
  _cs = 1;
  _spi.format(8, 0);
  _spi.frequency(30*100000);
  // _spi.frequency(30030030);
  _cs = 0;
  // ThisThread::sleep_for(200ms);

  hostCmd(HCMD_CLKEXT, 0x00);
  hostCmd(HCMD_CLKSEL, 0x46);
  hostCmd(HCMD_ACTIVE, 0x00);
  // ThisThread::sleep_for(300ms);

  // Check if EVE is in working status
  while (0x7c != read8(REG_ID))
    ;
  printf("EVE ID: 0x7c\n");
  while (0x0 != read16(REG_CPURESET))
    ;
  printf("EVE CPU reset: 0x0\n");

  // printf("%x\n", read32(0x0C0000)); // This should print "11708".

  write32(REG_FREQUENCY, 72000000);

  write8(REG_PCLK, 0); // Default to 0; we set it properly last
  write8(REG_PWM_DUTY, 0);


  // Panel timing registers000000
  write16(REG_HSIZE, p.width);
  write16(REG_HCYCLE, p.hCycle);
  write16(REG_HOFFSET, p.hOffset);
  write16(REG_HSYNC0, p.hSync0);
  write16(REG_HSYNC1, p.hSync1);
  write16(REG_VSIZE, p.height);
  write16(REG_VCYCLE, p.vCycle);
  write16(REG_VOFFSET, p.vOffset);
  write16(REG_VSYNC0, p.vSync0);
  write16(REG_VSYNC1, p.vSync1);
  write8(REG_SWIZZLE, 0);
  write8(REG_PCLK_POL, 1);

  write8(REG_CSPREAD, 0);
  write8(REG_DITHER, 0);
  write8(REG_DITHER, 0);

  // Write first display list
  write32(RAM_DL + 0, CLEAR_COLOR_RGB(0, 0, 0));
  write32(RAM_DL + 4, CLEAR(1, 1, 1));
  write32(RAM_DL + 8, DISPLAY);

  // Start graphics rendering process with initial display list
  write32(REG_DLSWAP, 2);

  write16(REG_GPIOX, read16(REG_GPIOX) | 0x8000);

  // Configure pixel clock
  write8(REG_PCLK, p.pclk);

  write16(REG_PWM_HZ, 250);

  write8(REG_PWM_DUTY, 128);

  // explicitly set vertex format for vertex2F
  cmd(VERTEX_FORMAT(4));
  loadFonts();

  // I dont think we need this?
  // Reset command FIFO
  // _cmd_wp = 0;
  // write32(REG_CMD_WRITE, 0);
  // ThisThread::sleep_for(200ms);
}

void BT817Q::loadFonts() {
  cmdLoadRomFonts(1, 34);
  cmdLoadRomFonts(2, 33);
  cmdLoadRomFonts(3, 32);
  // cmdLoadRomFonts(31, 34);
  // cmdLoadRomFonts(30, 33);
  // cmdLoadRomFonts(29, 32);
}

void BT817Q::selectWriteAddress(uint32_t addr) {
  const uint8_t txBuf[3] = {
      (uint8_t)((addr >> 16) | 0x80),
      (uint8_t)(addr >> 8),
      (uint8_t)(addr),
  };
  _spi.write(txBuf, sizeof(txBuf), nullptr, 0);
}

void BT817Q::selectReadAddress(uint32_t addr) {
  const uint8_t txBuf[4] = {
      (uint8_t)(addr >> 16),
      (uint8_t)(addr >> 8),
      (uint8_t)(addr),
      0x00,
  };
  _spi.write(txBuf, sizeof(txBuf), nullptr, 0);
}

void BT817Q::write8(uint32_t addr, uint8_t data) {
  _cs = 0;
  selectWriteAddress(addr);
  _spi.write(data);
  _cs = 1;
}

void BT817Q::write16(uint32_t addr, uint16_t data) {
  _cs = 0;
  selectWriteAddress(addr);
  _spi.write(data);
  _spi.write(data >> 8);
  _cs = 1;
}

void BT817Q::write32(uint32_t addr, uint32_t data) {
  _cs = 0;
  selectWriteAddress(addr);
  _spi.write(data);
  _spi.write(data >> 8);
  _spi.write(data >> 16);
  _spi.write(data >> 24);
  _cs = 1;
}

uint8_t BT817Q::read8(uint32_t addr) {
  _cs = 0;

  selectReadAddress(addr);
  uint8_t d = _spi.write(0x00);

  _cs = 1;
  return d;
}

uint16_t BT817Q::read16(uint32_t addr) {
  _cs = 0;

  selectReadAddress(addr);
  uint16_t d = _spi.write(0x00);
  d |= _spi.write(0x00) << 8;

  _cs = 1;
  return d;
}

uint32_t BT817Q::read32(uint32_t addr) {
  _cs = 0;

  selectReadAddress(addr);
  // uint32_t d = _spi.write(0x000000);
  uint32_t d = _spi.write(0x00);
  d |= _spi.write(0x00) << 8;
  d |= _spi.write(0x00) << 16;
  d |= _spi.write(0x00) << 24;

  _cs = 1;
  return d;
}

void BT817Q::hostCmd(uint8_t cmd, uint8_t param) {
  _cs = 0;
  const uint8_t buf[] = {cmd, param, 0x00};
  _spi.write(buf, sizeof(buf), nullptr, 0);
  _cs = 1;
  // ThisThread::sleep_for(20ms);
}

void BT817Q::cmd(uint32_t word) {
  write32(RAM_CMD + _cmd_wp, word);
  _cmd_wp = (_cmd_wp + 4) & 0x0FFF;
  write32(REG_CMD_WRITE, _cmd_wp);
}

// alternative implementation of cmd to test
//  void BT817Q::cmd(uint32_t word) {
//     uint32_t free_space = read32(REG_CMDB_SPACE);
//      if (free_space > 32) {
//          write32(REG_CMDB_WRITE, word);
//      }
//  }

void BT817Q::cmdString(const char *s) {
  // while (*s) cmd(*s++);
  // cmd(0);                          // zero‑terminate + pad
  // _cmd_wp = (_cmd_wp + 3) & 0x0FFC;
  // write32(REG_CMD_WRITE, _cmd_wp);

  _cs = 0;
  selectWriteAddress(RAM_CMD + _cmd_wp);

  uint8_t this_character;
  while (0 != (this_character = *s)) {
    _spi.write(this_character);
    s++;
    _cmd_wp = (_cmd_wp + 1) & 0xFFF;
  }

  // Send the mandatory null terminator
  _spi.write(0);
  // Keep track that we have written a byte
  _cmd_wp = (_cmd_wp + 1) & 0xFFF;

  // We need to ensure 4-byte alignment. Add nulls as necessary.
  while (0 != (_cmd_wp & 0x03)) {
    _spi.write(0);
    // Keep track that we have written a byte
    _cmd_wp = (_cmd_wp + 1) & 0xFFF;
  }

  _cs = 1;
}

// void BT817Q::cmdWait() {
//   while (read32(REG_CMD_READ) != _cmd_wp) {
//   }
// }

Result BT817Q::cmdWait() {
  if (read32(REG_CMD_READ) != _cmd_wp) {
    return failure;
  }
  return success;
}

Result BT817Q::startFrame() {
  if (cmdWait() == success) {
    _cmd_wp = 0;
    write32(REG_CMD_WRITE, 0);
    cmd(CMD_DLSTART);
    return success;
  }
  return failure;
}

void BT817Q::endFrame() {
  cmd(DISPLAY);
  cmd(CMD_SWAP);
  // cmdWait();

  // Start pixel clock *once* (enabled forever after first swap)
  if (read32(REG_PCLK) == 0) {
    write32(REG_PCLK, _p.pclk);
  }
}

void BT817Q::clear(uint8_t r, uint8_t g, uint8_t b) {
  cmd(CLEAR_COLOR_RGB(r, g, b));
  cmd(CLEAR(1, 1, 1));
}

void BT817Q::drawText(int16_t x, int16_t y, const char *s, uint8_t font, uint16_t options) {
  cmd(CMD_TEXT);
  cmd((uint32_t(y) << 16) | uint16_t(x));
  cmd((uint32_t(options) << 16) | font);
  cmdString(s);
}

void BT817Q::drawText(int16_t x, int16_t y, const char *s, Color color, uint8_t font, uint16_t options) {
  cmd(SAVE_CONTEXT());
  setMainColor(color);
  drawText(x, y, s, font, options);
  cmd(RESTORE_CONTEXT());
}

void BT817Q::drawNumber(int16_t x,
                        int16_t y,
                        uint32_t n,
                        int16_t font,
                        uint8_t width,
                        uint16_t options) {
  cmd(CMD_NUMBER);
  cmd((uint32_t(y) << 16) | uint16_t(x));
  cmd((uint32_t(options) | (width % 10)) << 16 | font);
  cmd(n);
}

void BT817Q::drawNumber(int16_t x,
                        int16_t y,
                        uint32_t n,
                        Color color,
                        int16_t font,
                        uint8_t width,
                        uint16_t options) {
  setMainColor(color);
  drawNumber(x, y, n, font, width, options);
}

void BT817Q::drawFormattedText(const int16_t x, const int16_t y, const char *s, const uint8_t font, const uint16_t options, ...) {
  const size_t len = snprintf(nullptr, 0, s);
  char buffer[len];
  va_list args;
  va_start(args, options);
  vsnprintf(buffer, len, s, args);
  va_end(args);
  drawText(x, y, buffer, font, options);
}

void BT817Q::drawFormattedText(int16_t x, int16_t y, const char *s, Color color, uint8_t font, uint16_t options, ...) {
  cmd(SAVE_CONTEXT());
  setMainColor(color);
  size_t len = snprintf(nullptr, 0, s);
  char buffer[len];
  va_list args;
  va_start(args, options);
  vsnprintf(buffer, len, s, args);
  va_end(args);
  drawText(x, y, buffer, font, options);
  cmd(RESTORE_CONTEXT());
}

void BT817Q::setBacklight(bool on) {
  uint8_t val = on ? 0xff : 0x00;
  write8(REG_GPIO_DIR, val);
  write8(REG_GPIO, val);
}

void BT817Q::setMainColor(Color color) {
  if (color.alpha != _current_alpha) {
    cmd(COLOR_A(color.alpha));
    _current_alpha = color.alpha;
  }
  cmd(COLOR_RGB(color.red, color.green, color.blue));
}

void BT817Q::setBackgroundColor(Color color) {
  cmd(CMD_BGCOLOR);
  cmd(color.red << 16 | color.green << 8 | color.blue);
}

void BT817Q::setForegroundColor(Color color) {
  cmd(CMD_FGCOLOR);
  cmd(color.red << 16 | color.green << 8 | color.blue);
}

/*
 * X and Y valid range from 0-511
 */
void BT817Q::drawPoint(uint16_t radius, uint16_t x, uint16_t y, Color color) {
  cmd(SAVE_CONTEXT());
  setMainColor(color);
  drawPoint(radius, x, y);
  cmd(RESTORE_CONTEXT());
}

/*
 * Uses default main color set by BT817Q::setMainColor()
 */
void BT817Q::drawPoint(uint16_t radius, uint16_t x, uint16_t y) {
  cmd(POINT_SIZE(radius));
  cmd(BEGIN(POINTS));
  cmd(VERTEX2F(x * 16, y * 16));
  cmd(END());
}

void BT817Q::drawRect(Point top_left,
                      Point bottom_right,
                      uint16_t corner_radius) {
  cmd(LINE_WIDTH(corner_radius));
  cmd(BEGIN(RECTS));
  cmd(VERTEX2F(top_left.x * 16, top_left.y * 16));
  cmd(VERTEX2F(bottom_right.x * 16, bottom_right.y * 16));
  cmd(END());
}

void BT817Q::drawRect(Point top_left,
                      Point bottom_right,
                      Color color,
                      uint16_t corner_radius) {
  cmd(SAVE_CONTEXT());
  setMainColor(color);
  drawRect(top_left, bottom_right, corner_radius);
  cmd(RESTORE_CONTEXT());
}

void BT817Q::drawLine(Point start, Point end, uint16_t width) {
  cmd(LINE_WIDTH(width));
  cmd(BEGIN(LINES));
  cmd(VERTEX2F(start.x * 16, start.y * 16));
  cmd(VERTEX2F(end.x * 16, end.y * 16));
  cmd(END());
}

void BT817Q::drawLine(Point start, Point end, Color color, uint16_t width) {
  cmd(SAVE_CONTEXT());
  setMainColor(color);
  drawLine(start, end, width);
  cmd(RESTORE_CONTEXT());
}

void BT817Q::drawGauge(Point pos,
                       int16_t r,
                       uint16_t val,
                       uint16_t range,
                       uint16_t major,
                       uint16_t minor,
                       uint16_t options) {
  cmd(CMD_GAUGE);
  cmd(uint32_t(pos.y) << 16 | pos.x);
  cmd(uint32_t(options) << 16 | r);
  cmd(uint32_t(minor) << 16 | major);
  cmd(uint32_t(range) << 16 | val);
}

void BT817Q::drawGauge(Point pos,
                       int16_t r,
                       uint16_t val,
                       uint16_t range,
                       Color bg,
                       Color fg,
                       uint16_t major,
                       uint16_t minor,
                       uint16_t options) {
  cmd(SAVE_CONTEXT());
  setBackgroundColor(bg);
  setMainColor(fg);
  drawGauge(pos, r, val, range, major, minor, options);
  cmd(RESTORE_CONTEXT());
}

void BT817Q::drawGauge(Point pos,
                       int16_t r,
                       uint16_t val,
                       uint16_t range,
                       Color bg,
                       Color majorTick,
                       Color minorTick,
                       Color needle,
                       uint16_t major,
                       uint16_t minor,
                       uint16_t options) {
  cmd(SAVE_CONTEXT());
  setBackgroundColor(bg);
  setMainColor(majorTick);
  drawGauge(pos,
            r,
            0,
            0,
            major,
            minor,
            OPT_FLAT | OPT_NOPOINTER); // gauge bg and minor ticks only
  if (minorTick.red != majorTick.red || minorTick.green != majorTick.green ||
      minorTick.blue != majorTick.blue) {
    setMainColor(minorTick);
    drawGauge(pos,
              r,
              0,
              0,
              major,
              1,
              OPT_FLAT | OPT_NOPOINTER | OPT_NOBACK); // gauge major ticks only
  }
  setMainColor(needle);
  drawGauge(pos,
            r,
            val,
            range,
            1,
            1,
            options | OPT_NOTICKS | OPT_NOBACK); // needle only
  cmd(RESTORE_CONTEXT());
}

void BT817Q::drawProgressBar(
    Point pos, int16_t w, int16_t h, uint16_t val, uint16_t range, bool flat) {
  uint16_t options = flat ? OPT_FLAT : 0;
  cmd(CMD_PROGRESS);
  cmd(uint32_t(pos.y) << 16 | pos.x);
  cmd(uint32_t(h) << 16 | w);
  cmd(uint32_t(val) << 16 | options);
  cmd(range);
}

void BT817Q::drawProgressBar(Point pos,
                             int16_t w,
                             int16_t h,
                             uint16_t val,
                             uint16_t range,
                             Color bg,
                             Color bar,
                             bool flat) {
  cmd(SAVE_CONTEXT());
  setBackgroundColor(bg);
  setMainColor(bar);
  drawProgressBar(pos, w, h, val, range, flat);
  cmd(RESTORE_CONTEXT());
}

void BT817Q::cmdLoadRomFonts(uint8_t handle, uint8_t font) {
  cmd(CMD_ROMFONT);
  cmd(uint32_t(handle));
  cmd(uint32_t(font));
}
