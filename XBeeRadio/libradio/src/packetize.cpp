#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <packetize.hpp>
#include <vector>

int unpacketize(uint8_t *data, Packet *packet) {

    if (packet->initialized == false) {
        packet->total_segments = data[0];
        packet->initialized = true;
        packet->id = data[3];
        packet->segments.reserve(packet->total_segments);
    }

    Segment *segment = &packet->segments[data[1]];

    memcpy(segment->segment_data, data, segment->data_size+4);
    if (segment->packet_id != packet->id) {
        printf("WARNING: Packets came out of order, data loss expected (ID %02x != %02x)\n", segment->packet_id, packet->id);
        return INVALID_PACKET;
    }

    return PACKETIZE_SUCCESS;
}

int packetize(uint8_t *data, int data_size, Packet *packet) {

    packet->total_segments =
        (data_size + (MAX_SEGMENT_DATA_LENGTH - 1)) / MAX_SEGMENT_DATA_LENGTH;
    packet->segments.reserve(packet->total_segments);
    packet->id = rand();


    int data_index = 0;
    for (int current_segment = 0; current_segment < packet->total_segments;
    current_segment++) {

        Segment *segment = &packet->segments[current_segment];
        segment->total_segments = packet->total_segments;
        segment->segment_number = current_segment;
        printf("Setting packet id for %d to %02x!\n", current_segment, packet->id);
        segment->packet_id = packet->id;
        int segment_data_size = 0;

        if (data_index >= data_size) {
            break;
        }

        for (; segment_data_size < MAX_SEGMENT_DATA_LENGTH; segment_data_size++) {
            if (data_index >= data_size) {
                break;
            }
            segment->data[segment_data_size] = data[data_index];
            data_index++;
        }

        segment->data_size = segment_data_size;

        // packet->segments[current_segment] = segment;

    }

    return PACKETIZE_SUCCESS;
}

std::vector<uint8_t> serialize(Segment *segment) {

    return std::vector<uint8_t>(segment->segment_data, segment->segment_data + segment->data_size + 4);

    // std::vector<uint8_t> bytes_to_send(segment->data_size + 4);
    // bytes_to_send = {segment->total_segments, segment->segment_number, segment->data_size};
    //
    // // printf("data size: %d\n", segment->data_size);
    // for (int i = 0; i < segment->data_size; i++) {
    //     bytes_to_send.push_back(segment->data[i]);
    // }
    //
    // return bytes_to_send;
    //
}

bool is_complete_packet(Packet *packet) {
    for (int i = 0; i < packet->total_segments; i++) {
        if (packet->segments[i].total_segments != packet->total_segments) {
            return false;
        }
    }
    return true;
}
