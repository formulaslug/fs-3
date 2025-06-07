#ifndef PACKETIZE
#define PACKETIZE

#include <cstdint>
#include <vector>

#define MAX_SEGMENT_DATA_LENGTH 90

enum PACKETIZE_STATUS {
    NOT_A_PACKET,
    INVALID_PACKET,
    PACKETIZE_SUCCESS
};

typedef union Segment {
    struct {
        uint8_t total_segments;
        uint8_t segment_number;
        uint8_t data_size;
        uint8_t packet_id;
        uint8_t data[MAX_SEGMENT_DATA_LENGTH];
    };
    uint8_t segment_data[4 + MAX_SEGMENT_DATA_LENGTH];
} Segment;

typedef struct Packet {
    std::vector<Segment> segments;
    uint8_t total_segments;
    uint8_t id;
    bool initialized;

    Packet(): segments(std::vector<Segment>()), total_segments(0), id(0), initialized(false) {}
} Packet;

int unpacketize(uint8_t data[], Packet *packet);
int packetize(uint8_t data[], int data_size, Packet *packet);
std::vector<uint8_t> serialize(Segment *segment);
bool is_complete_packet(Packet *packet);

#endif
