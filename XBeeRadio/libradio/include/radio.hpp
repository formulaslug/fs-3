#ifndef RADIO
#define RADIO

#include "DigitalIn.h"
#include "DigitalOut.h"
#define NETWORK_IDENTIFIER 0x1678

enum AT_RESPONSE {
    AT_OKAY,
    AT_CHECKSUM_INVALID,
};

#include "mbed.h"

enum AT_COMMAND {
    TEMPERATURE,
    DESTINATION_SET_LOW,
};

enum DELIVERY_STATUS {
    DELIVERY_SUCCESS = 0x00,
    NO_ACK_FAILURE = 0x01,
    CCA_FAILURE = 0x02,
    UNKNOWN,
    NO_RESPONSE,
};

class XBeeRadio {
public:
    XBeeRadio(SPI& spi, DigitalOut& csPin, DigitalIn& attnPin);

    int get_temp(void);
    int get_at_command(AT_COMMAND at_command, uint8_t* at_command_bytes);
    int send_at_command(AT_COMMAND at_command, uint8_t *parameters, uint parameters_size, uint8_t *resp_buf, uint resp_buf_size);
    int transmit(uint64_t destination, char *payload, int payload_size, uint8_t frameid);
    uint8_t calculate_checksum(const uint8_t *data, size_t length);

private:
    SPI& spi;
    DigitalOut& cs;
    DigitalIn& spi_attn;
};


#endif


