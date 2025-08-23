/* mbed Microcontroller Library
 * Copyright (c) 2025 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"
#include "radio.hpp"
#include "packetize.hpp"
#include <cstdint>
#include <vector>

SPI spi(PA_7, PA_6, PA_5); // mosi, miso, sclk[, ss]
DigitalOut cs(PB_0);
// changed from default on PCB due to nucleo solder bridges - see Slack message
DigitalIn spi_attn(PB_1);

void test_packetize() {

    uint8_t data[600] = { 0 };

    for (unsigned int i = 0; i < sizeof(data); i++) {
        data[i] = 'X';
    }

    data[0] = 'A';
    data[599] = 'B';

    Packet packet;
    packetize(data, sizeof(data), &packet);

    // printf("Total segments: %d\n", packet.total_segments);
    //
    // printf("WE ARE OUTSIDE THE FUNCTION!\n");
    //
    // for (int i = 0; i < packet.total_segments; i++) {
    //     printf("Segment %d (datasize: %d): ", i, packet.segments[i].data_size);
    //     for (int n = 0; n < packet.segments[i].data_size; n++) {
    //         printf("%c", packet.segments[i].data[n]);
    //     }
    //     printf("\n");
    // }
    //
    Packet deserialized_packet;
    for (int i = 0; i < packet.total_segments; i++) {
        printf("Segment %d (datasize: %d): ", i, packet.segments[i].data_size);


        vector<uint8_t> bytes_to_send = serialize(&packet.segments[i]);

        for (unsigned int n = 0; n < bytes_to_send.size(); n++) {
            printf("%02x ", bytes_to_send[n]);
        }
        printf("\n");

        printf("Packet number: %d\n", bytes_to_send[1]);

        int status = unpacketize(bytes_to_send.data(), &deserialized_packet);
        printf("status (%d)", status);
    }
    for (int i = 0; i < deserialized_packet.total_segments; i++) {
        printf("Segment %d (datasize: %d): ", i, deserialized_packet.segments[i].data_size);
        for (int n = 0; n < deserialized_packet.segments[i].data_size; n++) {
            printf("%c", deserialized_packet.segments[i].data[n]);
        }
        printf("\n");
    }

}

int main() {
    printf("Main() - LiveTelemetryMain abcdef\n");

    // test_packetize();
    // while (true) {};
    cs.write(1);

    spi.format(8, 0);
    spi.frequency(1000000);
    spi.set_default_write_value(0x00);

    XBeeRadio radio(spi, cs, spi_attn);
    ThisThread::sleep_for(200ms);

    // Turns out this wasn't even neccessary to receive data, also this causes seg fault
    // uint8_t parameters[] = { 0x00, 0x00, 0x00, 0x00 };
    // radio.send_at_command(DESTINATION_SET_LOW, parameters, sizeof(parameters), nullptr, 0);
    Packet deserialized_packet;
    int received_segment_count = 0;

    bool ready = false;
    int total_received = 0;
    Timer t;
    t.start();
    while (true) {

        if (spi_attn.read()) {
            continue;
        }

        cs.write(0);
        uint8_t start_buf[1] = { 0 };
        do {
            spi.write(nullptr, 0, start_buf, 1);
            // printf("%02x\n", start_buf[0]);
        } while (start_buf[0] != 0x7E);
        cs.write(1);

        uint8_t resp_buf[200] = {0x7E, 0};
        cs.write(0);
        spi.write(nullptr, 0, resp_buf + 1, sizeof(resp_buf));
        cs.write(1);
        // printf("Respbuf: %02x\n", resp_buf[9]);

        if (resp_buf[9] == 0) {
            // printf("Packet segment number #1 received, resetting.");
            received_segment_count = 0;
            deserialized_packet = {};
            ready = true;
        }

        // if (!ready) {
        //     continue;
        // }


        // printf("-%d dBm ", resp_buf[6]);
        int status = unpacketize(resp_buf+8, &deserialized_packet);
        if (status == INVALID_PACKET) {
            printf("Partial packet received!\n");
            // received_segment_count = 0;
            // deserialized_packet = {};
            // int status = unpacketize(resp_buf+8, &deserialized_packet);
            continue;
            // We should treat this as an entirely new packet
            // deserialized_packet = {};
            // received_segment_count = 0;
        }
        received_segment_count++;
        // printf("We just received %d segments\n ", received_segment_count);

        if (received_segment_count >= deserialized_packet.total_segments) {
            printf("Total segments %d\n", deserialized_packet.total_segments);
            printf("Is complete packet %d\n ", is_complete_packet(&deserialized_packet));
            for (int i = 0; i < deserialized_packet.total_segments; i++) {
                for (int n = 0; n < deserialized_packet.segments[i].data_size; n++) {

                    // total_received++;
                    // if (total_received > 10000) {
                    //     t.stop();
                    //     printf("%f\n", 10000.0 * 1000000 / ((t.elapsed_time().count())));
                    //     total_received = 0;
                    //     t.reset();
                    //     t.start();
                    // }
                    
                    printf("%02x", deserialized_packet.segments[i].data[n]);
                }
            }
            printf("\n");
            deserialized_packet = {};
            received_segment_count = 0;
        }

        // printf("Received data: \n");
        // for (uint i = 0; i < sizeof(resp_buf); i++) {
        //     printf("%02x ", resp_buf[i]);
        // }
        // printf("Data: ");
        // for (uint i = 8; i < sizeof(resp_buf); i++) {
        //     printf("%c", resp_buf[i]);
        // }


        // TODO: We need to "ack" the data
    }

    return 0;
}

