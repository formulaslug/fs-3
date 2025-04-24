#include "bt817/bt817_api.h"

/**************Private Include***********************/
#include "bt817/address_mapping.h"
#include "main.h"

/*************Private Define************************/

#define DISPLAY_LIST_MAX_SP 8191

#define addr_bytes(mode, addr)                                                 \
	{(((mode & 0x3) << 6) +                                                \
	  (addr >> 16)), /**first 2 bits are mode (read, write, cmd) and then  \
	                    bits 21:16 of addr */                              \
	 ((addr & 0xff00) >> 8), /*bits 15:8 of addr*/                         \
	 (addr & 0xff),          /*bits 7:0 of addr*/                          \
	 0x00}

#define ADDR_BUFSIZE 4

#ifndef SPI_TRANS_TIMEOUT
#define SPI_TRANS_TIMEOUT 500
#endif

#ifndef SPI_READ
// to make it easy to change which SPI implementation we're using and ensure
// consistent behavior between functions ex. to change from DMA to interrupts,
// we can change it once here instead of each individual read/write/command
// function
#define SPI_READ(buf, size)                                                    \
	HAL_SPI_Receive(&hspi1, buf, size, SPI_TRANS_TIMEOUT)
// using the blocking implementation of SPI since transactions need to be sent
// in order of completion, and the BT817 communicates in short enough
// transactions at a high enough data rate that blocking shouldn't be an issue
// (each transaction will likely only be a few microseconds)
#endif

#ifndef SPI_TRANSMIT
#define SPI_TRANSMIT(data, size)                                               \
	HAL_SPI_Transmit(&hspi1, data, size, SPI_TRANS_TIMEOUT)
#endif

#ifndef CS
// Make sure the pin and port here match the pin you're using for chip select on
// the board
#define CS(x) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, x)
#endif

#ifndef CS_ACTIVE
#define CS_ACTIVE GPIO_PIN_RESET
#endif

#ifndef CS_INACTIVE
#define CS_INACTIVE GPIO_PIN_SET
#endif

/*********API Implementation**************/
/**
 * Implementations of the functions defined in Section 1.4 of the Bridgetek
 * BT817 Programming Guide "API Reference Documentation" link to PDF:
 * https://www.mouser.com/catalog/additional/BRT_AN_033_BT81XSeriesProgrammingGuidev2.0.pdf
 * Page 11
 */
uint8_t rd8(uint32_t addr) {
	uint8_t read_addr[ADDR_BUFSIZE] = addr_bytes(0x0, addr);
	uint8_t read_buff[1];
	CS(CS_ACTIVE);
	SPI_TRANSMIT(read_addr, ADDR_BUFSIZE);
	SPI_READ(read_buff, 1);
	CS(CS_INACTIVE);
	return *read_buff;
}

uint16_t rd16(uint32_t addr) {
	uint8_t read_addr[ADDR_BUFSIZE] = addr_bytes(0x0, addr);
	uint8_t read_buff[2];
	CS(CS_ACTIVE);
	SPI_TRANSMIT(read_addr, ADDR_BUFSIZE);
	SPI_READ(read_buff, 2);
	CS(CS_INACTIVE);
	return (read_buff[0] << 8) + read_buff[1];
}

uint32_t rd32(uint32_t addr) {
	uint8_t read_addr[ADDR_BUFSIZE] = addr_bytes(0x0, addr);
	uint8_t read_buff[4];
	CS(CS_ACTIVE);
	SPI_TRANSMIT(read_addr, ADDR_BUFSIZE);
	SPI_READ(read_buff, 4);
	CS(CS_INACTIVE);
	return (read_buff[0] << 24) + (read_buff[1] << 16) +
	       (read_buff[2] << 8) + read_buff[3];
}

void wr8(uint32_t addr, uint8_t data) {
	uint8_t addr_buf[ADDR_BUFSIZE] = addr_bytes(0x2u, addr);
	CS(CS_ACTIVE);
	SPI_TRANSMIT(addr_buf, ADDR_BUFSIZE);
	SPI_TRANSMIT(&data, 1);
	CS(CS_INACTIVE);
}

void wr16(uint32_t addr, uint16_t data) {
	uint8_t addr_buf[ADDR_BUFSIZE] = addr_bytes(0x2u, addr);
	uint8_t write_buf[2] = {(data >> 8), (data & 0xFF)};
	CS(CS_ACTIVE);
	SPI_TRANSMIT(addr_buf, ADDR_BUFSIZE);
	SPI_TRANSMIT(write_buf, 2);
	CS(CS_INACTIVE);
}

void wr32(uint32_t addr, uint32_t data) {
	uint8_t addr_buf[ADDR_BUFSIZE] = addr_bytes(0x2u, addr);
	uint8_t write_buf[4] = {(data >> 24), (data & 0xFF0000) >> 16,
	                        (data & 0xFF00) >> 8, (data & 0xFF)};
	CS(CS_ACTIVE);
	SPI_TRANSMIT(addr_buf, ADDR_BUFSIZE);
	SPI_TRANSMIT(write_buf, 4);
	CS(CS_INACTIVE);
}

int dl(uint32_t cmd) {
	uint32_t dl_sp = rd32(REG_CMD_DL);
	if ((dl_sp + 4) > DISPLAY_LIST_MAX_SP)
		return -1; // display list is full, can't send more commands
	uint32_t write_addr = RAM_DL + dl_sp;
	wr32(write_addr, cmd);
	wr32(REG_CMD_DL, dl_sp + 4);
	return 0;
}

int cmd(uint32_t data) {
	uint32_t space = rd32(REG_CMDB_SPACE);
	if (space < 4)
		return -1; // no space
	wr32(REG_CMDB_WRITE, data);
	return 0;
}

void host_command(uint16_t cmd) {
	uint8_t transmit_buf[3] = {cmd >> 8, cmd & 0xFF, 0x00};
	CS(CS_ACTIVE);
	SPI_TRANSMIT(transmit_buf, 3);
	CS(CS_INACTIVE);
}