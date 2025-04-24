#ifndef __BT817_API_H
#define __BT817_API_H _

#include <stdint.h>

uint8_t rd8(uint32_t addr);

uint16_t rd16(uint32_t addr);

uint32_t rd32(uint32_t addr);

void wr8(uint32_t addr, uint8_t data);

void wr16(uint32_t addr, uint16_t data);

void wr32(uint32_t addr, uint32_t data);

int dl(uint32_t cmd);

int cmd(uint32_t data);

void host_command(uint16_t cmd);

#endif
