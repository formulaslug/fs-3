#include "LTC6811.h"

#include "mbed.h"
#include "rtos.h"

#include "LTC681xParallelBus.h"

#define TMP1075_I2C_ADDR 0x48
#define TMP1075_TEMP_REG 0x00

LTC6810::LTC6810(LTC681xBus &bus, uint8_t id) : m_bus(bus), m_id(id) {
  m_config =
    Configuration{.gpio5 = GPIOOutputState::kPassive,
                  .gpio4 = GPIOOutputState::kHigh,
                  .gpio3 = GPIOOutputState::kHigh,
                  .gpio2 = GPIOOutputState::kHigh,
                  .gpio1 = GPIOOutputState::kPassive,
                  .referencePowerOff = ReferencePowerOff::kAfterConversions,
                  .dischargeTimerEnabled = DischargeTimerEnable::kDisabled,
                  .adcMode = AdcModeOption::kDefault,
                  .undervoltageComparison = 0,
                  .overvoltageComparison = 0,
                  .dischargeState = {.value = 0},
                  .dischargeTimeout = DischargeTimeoutValue::kDisabled};
}

void LTC6810::updateConfig() {
  // Create configuration data to write
  uint8_t config[6];
  config[0] = (uint8_t) m_config.gpio5 << 7
    | (uint8_t) m_config.gpio4 << 6
    | (uint8_t) m_config.gpio3 << 5
    | (uint8_t) m_config.gpio2 << 4
    | (uint8_t) m_config.gpio1 << 3
    | (uint8_t) m_config.referencePowerOff << 2
    | (uint8_t) m_config.dischargeTimerEnabled << 1
    | (uint8_t) m_config.adcMode;
  config[1] = m_config.undervoltageComparison & 0xFF;
  config[2] = ((m_config.undervoltageComparison >> 8) & 0x0F)
    | (((uint8_t)m_config.overvoltageComparison & 0x0F) << 4);
  config[3] = (m_config.overvoltageComparison >> 4) & 0xFF;
  config[4] = m_config.dischargeState.value & 0xFF;
  config[5] = (((uint8_t)m_config.dischargeTimeout & 0x0F) << 4)
    | ((m_config.dischargeState.value >> 8) & 0x0F);

  auto cmd = LTC681xBus::BuildAddressedBusCommand(WriteConfigurationGroupA(), m_id);

  m_bus.SendDataCommand(cmd, config);
}

LTC6810::Configuration &LTC6810::getConfig() { return m_config; }

uint16_t *LTC6811::getVoltages() {
  auto cmd = StartCellVoltageADC(AdcMode::k7k, false, CellSelection::kAll);
  m_bus.SendCommand(LTC681xBus::BuildAddressedBusCommand(cmd, m_id));

  // Wait 2 ms for ADC to finish
  ThisThread::sleep_for(2ms); // TODO: Change

  // 4  * (Register of 6 Bytes + PEC)
  uint8_t rxbuf[8 * 2];

  m_bus.SendReadCommand(LTC681xBus::BuildAddressedBusCommand(ReadCellVoltageGroupA(), m_id), rxbuf);
  m_bus.SendReadCommand(LTC681xBus::BuildAddressedBusCommand(ReadCellVoltageGroupB(), m_id), rxbuf + 8);
  m_bus.SendReadCommand(LTC681xBus::BuildAddressedBusCommand(ReadCellVoltageGroupC(), m_id), rxbuf + 16);
  m_bus.SendReadCommand(LTC681xBus::BuildAddressedBusCommand(ReadCellVoltageGroupD(), m_id), rxbuf + 24);

  // Voltage = val • 100μV
  uint16_t *voltages = new uint16_t[6];
  for (unsigned int i = 0; i < sizeof(rxbuf); i++) {
    // Skip over PEC
    if (i % 8 == 6 || i % 8 == 7) continue;

    // Skip over odd bytes
    if (i % 2 == 1) continue;

    // Wack shit to skip over PEC
    voltages[(i / 2) - (i / 8)] = ((uint16_t)rxbuf[i]) | ((uint16_t)rxbuf[i + 1] << 8);
  }

  return voltages;
}

uint16_t *LTC6810::getGpio() {
  auto cmd = StartGpioADC(AdcMode::k7k, GpioSelection::kAll);
  m_bus.SendCommand(LTC681xBus::BuildAddressedBusCommand(cmd, m_id));

  // Wait 15 ms for ADC to finish
  ThisThread::sleep_for(5ms); // TODO: This could be done differently

  uint8_t rxbuf[8 * 2];

  m_bus.SendReadCommand(LTC681xBus::BuildAddressedBusCommand(ReadAuxiliaryGroupA(), m_id), rxbuf);
  m_bus.SendReadCommand(LTC681xBus::BuildAddressedBusCommand(ReadAuxiliaryGroupB(), m_id), rxbuf + 8);

  uint16_t *voltages = new uint16_t[5];

  for (unsigned int i = 0; i < sizeof(rxbuf); i++) {
    // Skip over PEC
    if (i % 8 == 6 || i % 8 == 7) continue;

    // Skip over odd bytes
    if (i % 2 == 1) continue;

    // Wack shit to skip over PEC
    voltages[(i / 2) - (i / 8)] = ((uint16_t)rxbuf[i]) | ((uint16_t)rxbuf[i + 1] << 8);
  }

  return voltages;
}

uint16_t *LTC6810::getGpioPin(GpioSelection pin) {
  auto cmd = StartGpioADC(AdcMode::k7k, pin);
  m_bus.SendCommand(LTC681xBus::BuildAddressedBusCommand(cmd, m_id));

  // Wait 5 ms for ADC to finish
  ThisThread::sleep_for(pin == GpioSelection::kAll ? 15ms : 5ms); // TODO: Change to polling

  uint8_t rxbuf[8 * 2];

  m_bus.SendReadCommand(LTC681xBus::BuildAddressedBusCommand(ReadAuxiliaryGroupA(), m_id), rxbuf);
  m_bus.SendReadCommand(LTC681xBus::BuildAddressedBusCommand(ReadAuxiliaryGroupB(), m_id), rxbuf + 8);

  uint16_t *voltages = new uint16_t[5];

  for (unsigned int i = 0; i < sizeof(rxbuf); i++) {
    // Skip over PEC
    if (i % 8 == 6 || i % 8 == 7) continue;

    // Skip over odd bytes
    if (i % 2 == 1) continue;

    // Wack shit to skip over PEC
    voltages[(i / 2) - (i / 8)] = ((uint16_t)rxbuf[i]) | ((uint16_t)rxbuf[i + 1] << 8);
  }

  return voltages;
}

void LTC6810::buildCOMMBytes(uint8_t icom, uint8_t fcom, uint8_t data, uint8_t *commBytes) {
  // COMMn (even byte): Upper 4 bits = ICOM, Lower 4 bits = upper half of data
  commBytes[0] = (icom << 4) | ((data >> 4) & 0x0F);
  
  // COMMn+1 (odd byte): Upper 4 bits = lower half of data, Lower 4 bits = FCOM
  commBytes[1] = ((data & 0x0F) << 4) | fcom;
}

float LTC6810::readTemperatureTMP1075() {
  uint8_t commData[6]
  uint8_t rxData[8]; 
  uint8_t tempBytes[2];

  buildCOMMBytes(0x6, 0x0, (TMP1075_I2C_ADDR << 1) | 0x00, tempBytes);
  commData[0] = tempBytes[0];  // COMM0
  commData[1] = tempBytes[1];  // COMM1

  buildCOMMBytes(0x0, 0x0, TMP1075_TEMP_REG, tempBytes);
  commData[2] = tempBytes[0];  // COMM2
  commData[3] = tempBytes[1];  // COMM3

  buildCOMMBytes(0x1, 0x0, 0x00, tempBytes);
  commData[4] = tempBytes[0];  // COMM4
  commData[5] = tempBytes[1];  // COMM5

  auto wrCmd = LTC681xBus::BuildAddressedBusCommand(WriteCommGroup(), m_id);
  m_bus.SendDataCommand(wrCmd, commData);
  
  auto stCmd = LTC681xBus::BuildAddressedBusCommand(StartComm(), m_id);
  m_bus.SendCommand(stCmd);

  ThisThread::sleep_for(3ms);

  buildCOMMBytes(0x6, 0x0, (TMP1075_I2C_ADDR << 1) | 0x01, tempBytes);
  commData[0] = tempBytes[0];  // COMM0
  commData[1] = tempBytes[1];  // COMM1


  buildCOMMBytes(0x0, 0x0, 0xFF, tempBytes);
  commData[2] = tempBytes[0];  // COMM2
  commData[3] = tempBytes[1];  // COMM3

  buildCOMMBytes(0x0, 0x9, 0xFF, tempBytes);
  commData[4] = tempBytes[0];  // COMM4
  commData[5] = tempBytes[1];  // COMM5

  m_bus.SendDataCommand(wrCmd, commData);
  m_bus.SendCommand(stCmd);

  ThisThread::sleep_for(3ms);

  auto rdCmd = LTC681xBus::BuildAddressedBusCommand(ReadCommGroup(), m_id);
  m_bus.SendReadCommand(rdCmd, rxData);

  uint8_t tempMSB = ((rxData[2] & 0x0F) << 4) | ((rxData[3] >> 4) & 0x0F);

  uint8_t tempLSB = ((rxData[4] & 0x0F) << 4) | ((rxData[5] >> 4) & 0x0F);
  
  int16_t rawTemp = (tempMSB << 8) | tempLSB;

  float temperature = (rawTemp >> 4) * 0.0625f;

  return temperature;
}

bool LTC6810::verifyI2CStatus(uint8_t *rxData) {
  uint8_t data1 = ((rxData[2] & 0x0F) << 4) | ((rxData[3] >> 4) & 0x0F);
  uint8_t data2 = ((rxData[4] & 0x0F) << 4) | ((rxData[5] >> 4) & 0x0F);
  
  if (data1 == 0xFF && data2 == 0xFF) {
    return false;  // No response from sensor
  }
  
  return true;
}

