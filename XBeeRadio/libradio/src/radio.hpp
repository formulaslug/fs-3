#ifndef RADIO
#define RADIO

#include "DigitalIn.h"
#include "DigitalOut.h"

enum AT_RESPONSE {
    AT_OKAY,
    AT_CHECKSUM_INVALID,
};

#include "mbed.h"

enum AT_COMMAND {
    TEMPERATURE,
    MAX_TRANSMIT_SIZE,
    DESTINATION_SET_LOW,
    BAUD_RATE_SET,
    BROADCAST_MULTI_TRANSMITS,
    NETWORK_IDENTIFIER,
    NETWORK_HOPS,
    STREAMING_LIMIT,
    TRANSMIT_OPTIONS,
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

    int16_t get_temp(void);
    int baud_rate_set(uint8_t baud_rate);
    int set_repeat_transmissions(uint8_t repeat_count);
    int set_network_hops(uint8_t network_hops);
    int set_streaming_limit(uint8_t streaming_limit);
    int set_network_identifier(char* network_identifier, int network_identifier_size);
    int16_t get_max_transmit_size(void);
    int get_at_command(AT_COMMAND at_command, uint8_t* at_command_bytes);
    int send_at_command(AT_COMMAND at_command, uint8_t *parameters, uint parameters_size, uint8_t *resp_buf, uint resp_buf_size);
    int transmit(uint8_t *payload, int payload_size);
    int transmit_raw(uint64_t destination, uint8_t *payload, int payload_size, uint8_t frameid);
    uint8_t calculate_checksum(const uint8_t *data, size_t length);

private:
    SPI& spi;
    DigitalOut& cs;
    DigitalIn& spi_attn;
};


#endif


