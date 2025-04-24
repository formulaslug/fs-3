#include "bt817/bt817_api.h"

/**************Private Include***********************/
#include "bt817/address_mapping.h"
#include "main.h"


/*************Private Define************************/

#define DISPLAY_LIST_MAX_SP 8191
#define ADDR_BUFSIZE 4
/**first 2 bits are mode (read, write, cmd) and then bits 21:16 of addr */
#define ADDR(mode, addr)														\
	uint8_t ___byte1 = (((mode & 0x3) << 6) + (addr >> 16));					\
	uint8_t ___byte2 =	 ((addr & 0xff00) >> 8); /*bits 15:8 of addr*/          \
	uint8_t ___byte3 = (addr & 0xFF);											\
	uint8_t addr_buf[ADDR_BUFSIZE] {___byte1, ___byte2, ___byte3, 0x00 }

#define MODE_RD 0x0
#define MODE_WR 0x2U

#ifndef DBG_A
#define DBG_A(msg)
#endif

#ifndef SPI_TRANS_TIMEOUT
#define SPI_TRANS_TIMEOUT 500
#endif

#ifndef SPI_READ
// to make it easy to change which SPI implementation we're using and ensure
// consistent behavior between functions ex. to change from DMA to interrupts,
// we can change it once here instead of each individual read/write/command
// function
#define SPI_READ(addr_buf, buf, size)                                                    \
	spi.write(addr_buf, ADDR_BUFSIZE, buf, size)
	//HAL_SPI_Receive(&hspi3, buf, size, SPI_TRANS_TIMEOUT)
// using the blocking implementation of SPI since transactions need to be sent
// in order of completion, and the BT817 communicates in short enough
// transactions at a high enough data rate that blocking shouldn't be an issue
// (each transaction will likely only be a few microseconds)
#endif

#ifndef SPI_TRANSMIT
#define SPI_TRANSMIT(data, size)                                               \
	spi.write(data, size, NULL, 0)
	//HAL_SPI_Transmit(&hspi3, data, size, SPI_TRANS_TIMEOUT)
#endif

#ifndef CS
// Make sure the pin and port here match the pin you're using for chip select on
// the board
//#define CS(x) HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, x)
#define CS(x) dash_cs = x
#endif

#ifndef CS_ACTIVE
#define CS_ACTIVE 0
#endif

#ifndef CS_INACTIVE
#define CS_INACTIVE 1
#endif

/*********API Implementation**************/
/**
 * Implementations of the functions defined in Section 1.4 of the Bridgetek
 * BT817 Programming Guide "API Reference Documentation" link to PDF:
 * https://www.mouser.com/catalog/additional/BRT_AN_033_BT81XSeriesProgrammingGuidev2.0.pdf
 * Page 11
 */
uint8_t rd8(uint32_t addr) {
	// uint8_t read_addr[ADDR_BUFSIZE] addr_bytes(0x0, addr);
	ADDR(MODE_RD, addr);
	uint8_t read_buf[1];
	CS(CS_ACTIVE);
	//SPI_TRANSMIT(read_addr, ADDR_BUFSIZE);
	//SPI_READ(read_buff, 1);
	SPI_READ(read_buf, read_buf, 1);
	CS(CS_INACTIVE);
	return *read_buf;
}

uint16_t rd16(uint32_t addr) {
	// uint8_t read_addr[ADDR_BUFSIZE] = addr_bytes(0x0, addr);
	ADDR(MODE_RD, addr);
	uint8_t read_buf[2];
	CS(CS_ACTIVE);
	//SPI_TRANSMIT(read_addr, ADDR_BUFSIZE);
	//SPI_READ(read_buff, 2);
	SPI_READ(read_buf, read_buf, 2);
	CS(CS_INACTIVE);
	return (read_buf[0] << 8) + read_buf[1];
}

uint32_t rd32(uint32_t addr) {
	// uint8_t read_addr[ADDR_BUFSIZE] = addr_bytes(0x0, addr);
	ADDR(MODE_RD, addr);
	uint8_t read_buf[4];
	CS(CS_ACTIVE);
	//SPI_TRANSMIT(read_addr, ADDR_BUFSIZE);
	//SPI_READ(read_buff, 4);
	SPI_READ(addr_buf, read_buf, 4);
	CS(CS_INACTIVE);
	return (read_buf[0] << 24) + (read_buf[1] << 16) +
	       (read_buf[2] << 8) + read_buf[3];
}

void wr8(uint32_t addr, uint8_t data) {
	// uint8_t addr_buf[ADDR_BUFSIZE] = addr_bytes(0x2u, addr);
	ADDR(MODE_WR, addr);
	CS(CS_ACTIVE);
	SPI_TRANSMIT(addr_buf, ADDR_BUFSIZE);
	SPI_TRANSMIT(&data, 1);
	CS(CS_INACTIVE);
}

void wr16(uint32_t addr, uint16_t data) {
	// uint8_t addr_buf[ADDR_BUFSIZE] = addr_bytes(0x2u, addr);
	ADDR(MODE_WR, addr);
	uint8_t byte1 = data>>8;
	uint8_t byte2 = data & 0xFF;
	// uint8_t write_buf[2] = {(data >> 8), (data & 0xFF)};
	uint8_t write_buf[2] {byte1, byte2};
	CS(CS_ACTIVE);
	SPI_TRANSMIT(addr_buf, ADDR_BUFSIZE);
	SPI_TRANSMIT(write_buf, 2);
	CS(CS_INACTIVE);
}

void wr32(uint32_t addr, uint32_t data) {
	// uint8_t addr_buf[ADDR_BUFSIZE] = addr_bytes(0x2u, addr);
	ADDR(MODE_WR, addr);
	uint8_t byte1 = data >>24;
	uint8_t byte2 = (data & 0xFF0000) >> 16;
	uint8_t byte3 =	(data & 0xFF00) >> 8;
	uint8_t byte4 = data & 0xFF;
	uint8_t write_buf[4] {byte1, byte2, byte3, byte4};
	// uint8_t write_buf[4] = {(data >> 24), (data & 0xFF0000) >> 16,
	                        // (data & 0xFF00) >> 8, (data & 0xFF)};
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
	if (space < 4) {
	    DBG_A("No space in command queue\r\n");
	    return -1; // no space
	}
	wr32(REG_CMDB_WRITE, data);
	return 0;
}

void host_command(uint16_t cmd) {
	uint8_t byte1 = cmd >> 8;
	uint8_t byte2 = cmd & 0xFF;
	// uint8_t transmit_buf[3] = {cmd >> 8, cmd & 0xFF, 0x00};
	uint8_t transmit_buf[3] {byte1, byte2, 0x00};
	CS(CS_ACTIVE);
	SPI_TRANSMIT(transmit_buf, 3);
	CS(CS_INACTIVE);
}