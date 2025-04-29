#include "BT817Q.hpp"

BT817Q::BT817Q(PinName mosi, PinName miso, PinName sck,
               PinName cs,   PinName pdn, PinName irq,
               EvePanel panel)
    : _spi(mosi, miso, sck),
      _cs(cs),
      _pdn(pdn),
      _irq(irq),
      _p(panel)
      {}

void BT817Q::init(const EvePanel &p) {
    printf("Initializing EVE...\n");
    // ThisThread::sleep_for(1000ms);

    ThisThread::sleep_for(100ms);
    _pdn = 0;
    ThisThread::sleep_for(100ms);
    _pdn = 1;
    ThisThread::sleep_for(100ms);

    // SPI safe‑start (<= 11 MHz until PCLK up)
    _cs = 1;
    _spi.format(8, 0);
    _spi.frequency(1000000);
    // _spi.frequency(30030030);
    _cs = 0;

    hostCmd(HCMD_CLKEXT, 0x00);
    hostCmd(HCMD_CLKSEL, 0x46);
    hostCmd(HCMD_ACTIVE, 0x00);
    ThisThread::sleep_for(300ms);

    // Check if EVE is in working status
    while (0x7c != read8(REG_ID));
    printf("EVE ID: 0x7c\n");
    while (0x0 != read16(REG_CPURESET));
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


    // I dont think we need this?
    // Reset command FIFO
    // _cmd_wp = 0;
    // write32(REG_CMD_WRITE, 0);
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

void BT817Q::cmdString(const char *s) {
    // while (*s) cmd(*s++);
    // cmd(0);                          // zero‑terminate + pad
    // _cmd_wp = (_cmd_wp + 3) & 0x0FFC;
    // write32(REG_CMD_WRITE, _cmd_wp);

    _cs = 0;
    selectWriteAddress(RAM_CMD + _cmd_wp);

    uint8_t this_character;
    while(0 != (this_character=*s)) {
        _spi.write(this_character);
        s++;
        _cmd_wp=(_cmd_wp+1)&0xFFF;
    }

    //Send the mandatory null terminator
    _spi.write(0);
    //Keep track that we have written a byte
    _cmd_wp=(_cmd_wp+1)&0xFFF;

    //We need to ensure 4-byte alignment. Add nulls as necessary.
    while(0 != (_cmd_wp&0x03))
        {
        _spi.write(0);
        //Keep track that we have written a byte
        _cmd_wp=(_cmd_wp+1)&0xFFF;
    }

    _cs = 1;
}

void BT817Q::cmdWait() {
    while (read32(REG_CMD_READ) != _cmd_wp) { }
}

void BT817Q::startFrame() {
    _cmd_wp = 0;
    write32(REG_CMD_WRITE, 0);
    cmd(CMD_DLSTART);
}

void BT817Q::endFrame() {
    cmd(DISPLAY);
    cmd(CMD_SWAP);
    cmdWait();

    // Start pixel clock *once* (enabled forever after first swap)
    if (read32(REG_PCLK) == 0) {
        write32(REG_PCLK, _p.pclk);
    }
}

void BT817Q::clear(uint8_t r, uint8_t g, uint8_t b) {
    cmd(CLEAR_COLOR_RGB(r, g, b));
    cmd(CLEAR(1, 1, 1));
}

void BT817Q::drawText(int16_t x, 
                      int16_t y,
                      uint8_t font, const char *s,
                      uint16_t options) {
    cmd(CMD_TEXT);
    cmd((uint32_t(y) << 16) | uint16_t(x));
    cmd((uint32_t(options) << 16) | font);
    cmdString(s);
}

void BT817Q::setBacklight(bool on) {
    uint8_t val = on ? 0xff : 0x00;
    write8(REG_GPIO_DIR, val);
    write8(REG_GPIO, val);
}
