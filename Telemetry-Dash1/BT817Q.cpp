#include "BT817Q.hpp"

BT817Q::BT817Q(PinName mosi, PinName miso, PinName sck,
               PinName cs,   PinName pdn, PinName irq)
    : _spi(mosi, miso, sck),
      _cs(cs),
      _pdn(pdn),
      _irq(irq) {}

void BT817Q::hostCmd(uint8_t cmd) {
    _cs = 0;
    _spi.write(cmd);
    _cs = 1;
    ThisThread::sleep_for(20);
}

void BT817Q::write8(uint32_t addr, uint8_t data) {
    _cs = 0;
    _spi.write(addr >> 16);            // MSB 0 = write
    _spi.write(addr >> 8);
    _spi.write(addr);
    _spi.write(data);
    _cs = 1;
}

void BT817Q::write16(uint32_t addr, uint16_t data) {
    _cs = 0;
    _spi.write(addr >> 16);            // MSB 0 = write
    _spi.write(addr >> 8);
    _spi.write(addr);
    _spi.write(data);
    _spi.write(data >> 8);
    _cs = 1;
}

void BT817Q::write32(uint32_t addr, uint32_t data) {
    _cs = 0;
    _spi.write(addr >> 16);            // MSB 0 = write
    _spi.write(addr >> 8);
    _spi.write(addr);
    _spi.write(data);
    _spi.write(data >> 8);
    _spi.write(data >> 16);
    _spi.write(data >> 24);
    _cs = 1;
}

uint8_t BT817Q::read8(uint32_t addr) {
    _cs = 0;
    _spi.write((addr >> 16) | 0x80);   // MSB 1 = read
    _spi.write(addr >> 8);
    _spi.write(addr);
    uint32_t d = _spi.write(0);
    _cs = 1;
    return d;
}

uint16_t BT817Q::read16(uint32_t addr) {
    _cs = 0;
    _spi.write((addr >> 16) | 0x80);   // MSB 1 = read
    _spi.write(addr >> 8);
    _spi.write(addr);
    uint32_t d = _spi.write(0) |
                 (_spi.write(0) << 8);
    _cs = 1;
    return d;
}

uint32_t BT817Q::read32(uint32_t addr) {
    _cs = 0;
    _spi.write((addr >> 16) | 0x80);   // MSB 1 = read
    _spi.write(addr >> 8);
    _spi.write(addr);
    uint32_t d = _spi.write(0) |
                 (_spi.write(0) << 8) |
                 (_spi.write(0) << 16) |
                 (_spi.write(0) << 24);
    _cs = 1;
    return d;
}

void BT817Q::cmd(uint32_t word) {
    write32(RAM_CMD + _cmd_wp, word);
    _cmd_wp = (_cmd_wp + 4) & 0x0FFF;
    write32(REG_CMD_WRITE, _cmd_wp);
}

void BT817Q::cmdString(const char *s) {
    while (*s) cmd(*s++);
    cmd(0);                          // zero‑terminate + pad
    _cmd_wp = (_cmd_wp + 3) & 0x0FFC;
    write32(REG_CMD_WRITE, _cmd_wp);
}

void BT817Q::cmdWait() {
    while (read32(REG_CMD_READ) != _cmd_wp) { }
}

void BT817Q::init(const EvePanel &p) {
    printf("Initializing EVE...\n");
    // SPI safe‑start (<= 11 MHz until PCLK up)
    _cs = 1;
    _spi.format(8, 3);
    _spi.frequency(1000000);
    _cs = 0;

    hostCmd(HCMD_ACTIVE);
    ThisThread::sleep_for(300ms);

    // Check if EVE is in working status
    while (0x7c != read8(REG_ID));
    printf("EVE ID: 0x7c\n");
    while (0x0 != read16(REG_CPURESET));
    printf("EVE CPU reset: 0x0\n");

    write32(REG_FREQUENCY, 0x3938700);

    // Panel timing registers
    write16(REG_HCYCLE, p.hCycle);
    write16(REG_HOFFSET, p.hOffset);
    write16(REG_HSYNC0, p.hSync0);
    write16(REG_HSYNC1, p.hSync1);
    write16(REG_VCYCLE, p.vCycle);
    write16(REG_VOFFSET, p.vOffset);
    write16(REG_VSYNC0, p.vSync0);
    write16(REG_VSYNC1, p.vSync1);
    write8(REG_SWIZZLE, 0);
    write8(REG_PCLK_POL, 1);
    write8(REG_CSPREAD, 0);
    write16(REG_HSIZE, p.width);
    write16(REG_VSIZE, p.height);

    // Write first display list
    write32(RAM_DL + 0, CLEAR_COLOR_RGB(0, 0, 0));
    write32(RAM_DL + 4, CLEAR(1, 1, 1));
    write32(RAM_DL + 8, DISPLAY);
    // Display list swap
    cmd(CMD_SWAP);
    cmdWait();

    // Backlight GPIO0 off (user enables later)
    setBacklight(false);

    write8(REG_PCLK, p.pclk);

    // Reset command FIFO
    _cmd_wp = 0;
    write32(REG_CMD_WRITE, 0);
}

void BT817Q::startFrame() {
    _cmd_wp = 0;
    write32(REG_CMD_WRITE, 0);
    cmd(CMD_DLSTART);
}

void BT817Q::clear(uint8_t r, uint8_t g, uint8_t b) {
    cmd(CLEAR_COLOR_RGB(r, g, b));
    cmd(CLEAR(1, 1, 1));
}

void BT817Q::drawText(int16_t x, 
                      int16_t y,
                      uint8_t font,
                      const char *s, 
                      uint16_t options) {
    cmd(CMD_TEXT);
    cmd((uint32_t(y) << 16) | uint16_t(x));
    cmd((uint32_t(font) << 16) | options);
    cmdString(s);
}

void BT817Q::endFrame() {
    cmd(DISPLAY);
    cmd(CMD_SWAP);
    cmdWait();

    // Start pixel clock *once* (enabled forever after first swap)
    if (read32(REG_PCLK) == 0) {
        write32(REG_PCLK, 2);          // TODO: parametric (use panel.pclk)
    }
}

void BT817Q::setBacklight(bool on) {
    uint8_t val = on ? 0xff : 0x00;
    write8(REG_GPIO_DIR, val);
    write8(REG_GPIO, val);
}