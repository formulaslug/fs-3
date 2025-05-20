#ifndef RADIO
#define RADIO

#include "DigitalIn.h"
#include "DigitalOut.h"
#define NETWORK_IDENTIFIER 0x1678

#include "mbed.h"

enum AT_RESPONSE {
    AT_OKAY,
    AT_CHECKSUM_INVALID,
};

enum AT_COMMAND {
    TEMPERATURE
};

class XBeeRadio {
public:
    XBeeRadio(SPI& spi, DigitalOut& csPin, DigitalIn& attnPin);

    int get_temp(void);
    int get_at_command(AT_COMMAND at_command, uint8_t* at_command_bytes);
    int send_at_command(AT_COMMAND at_command, uint8_t *parameters, uint parameters_size, uint8_t *resp_buf, uint resp_buf_size);
    int transmit(uint64_t destination, char *payload);
    uint8_t calculate_checksum(const uint8_t *data, size_t length);

private:
    SPI& spi;
    DigitalOut& cs;
    DigitalIn& spi_attn;
};


#endif


