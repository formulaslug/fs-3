#include "DS18B20.h"
#include "mbed.h"

void debug_search_for_ds18b20_address(OneWire& bus) {
    uint8_t addr[8];

    if (!bus.search(addr)) {
        printf("No more addresses.\r\n\r\n");
        bus.reset_search();
        ThisThread::sleep_for(250ms);
        return;
    }

    printf("ROM = ");
    for (uint8_t byte : addr) {
        printf(" %x", byte);
    }

    if (OneWire::crc8(addr, 7) != addr[7]) {
        printf("CRC is not valid!\r\n\r\n");
        return;
    }

    // the first ROM byte indicates which chip
    switch (addr[0]) {
    case 0x10:
        printf("  Chip = DS18S20 (type_s) = 1\r\n"); // or old DS1820
        break;
    case 0x28:
        printf("  Chip = DS18B20 (type_s) = 0\r\n");
        break;
    case 0x22:
        printf("  Chip = DS1822  (type_s) = 0\r\n");
        break;
    default:
        printf("Device is not a DS18x20 family device.\r\n");
        return;
    }
}

DS18B20::DS18B20(OneWire& onewire_bus, uint64_t device_address)
    : bus(onewire_bus), address(device_address) {}

void DS18B20::start_conversion(bool parasite_power_mode) {
    bus.reset();
    bus.select((uint8_t*)&address);
    bus.write(0x44, parasite_power_mode ? 1 : 0); // start conversion, with parasite power on at the end
}

uint8_t DS18B20::retrieve_conversion(bool type_s_sensor) {
    // we might do a ds18b20.depower() here, but the reset will take care of it.

    /* uint8_t present = */ bus.reset();
    bus.select((uint8_t*)&address);
    bus.write(0xBE); // Read Scratchpad

    // printf("  Data = %x \r\n\r\n", present);
    uint8_t data[12];
    for (int i = 0; i < 9; i++) { // we need 9 bytes
        data[i] = bus.read();
        // printf(" %x", data[i]);
    }
    // printf("\r\n");
    // printf(" CRC= %x \r\n\r\n", OneWire::crc8(data, 8));

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = ((data[1] << 8) | data[0]);
    if (type_s_sensor) {
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10) {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
    } else {
        uint8_t cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00)
            raw = raw & ~7; // 9 bit resolution, 93.75 ms
        else if (cfg == 0x20)
            raw = raw & ~3; // 10 bit res, 187.5 ms
        else if (cfg == 0x40)
            raw = raw & ~1; // 11 bit res, 375 ms
                            //// default is 12 bit resolution, 750 ms conversion time
    }
    uint8_t celsius_x2 = raw / 8;
    // float celsius = (float)raw / 16.0;
    // fahrenheit = celsius * 1.8 + 32.0;
    return celsius_x2;
}
