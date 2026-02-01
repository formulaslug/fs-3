#include "LTC6811.h"

#include "mbed.h"
#include "rtos.h"

#include "LTC681xParallelBus.h"


LTC6811::LTC6811(LTC681xBus &bus, uint8_t id) : m_bus(bus), m_id(id) {
  m_config =
    Configuration{.gpio5 = GPIOOutputState::kHigh,
                  .gpio4 = GPIOOutputState::kHigh,
                  .gpio3 = GPIOOutputState::kPassive,
                  .gpio2 = GPIOOutputState::kPassive,
                  .gpio1 = GPIOOutputState::kPassive,
                  .referencePowerOff = ReferencePowerOff::kAfterConversions,
                  .dischargeTimerEnabled = DischargeTimerEnable::kDisabled,
                  .adcMode = AdcModeOption::kDefault,
                  .undervoltageComparison = 0,
                  .overvoltageComparison = 0,
                  .dischargeState = {.value = 0},
                  .dischargeTimeout = DischargeTimeoutValue::kDisabled};
}

void LTC6811::updateConfig() {
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

LTC6811::Configuration &LTC6811::getConfig() { return m_config; }

uint16_t *LTC6811::getVoltages() {
  auto cmd = StartCellVoltageADC(AdcMode::k7k, false, CellSelection::kAll);
  m_bus.SendCommand(LTC681xBus::BuildAddressedBusCommand(cmd, m_id));

  // Wait 2 ms for ADC to finish
  ThisThread::sleep_for(2ms); // TODO: Change

  // 4  * (Register of 6 Bytes + PEC)
  uint8_t rxbuf[8 * 2];

  m_bus.SendReadCommand(LTC681xBus::BuildAddressedBusCommand(ReadCellVoltageGroupA(), m_id), rxbuf);
  m_bus.SendReadCommand(LTC681xBus::BuildAddressedBusCommand(ReadCellVoltageGroupB(), m_id), rxbuf + 8);
 

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

uint16_t *LTC6811::getGpio() {
  auto cmd = StartGpioADC(AdcMode::k7k, GpioSelection::kAll);
  m_bus.SendCommand(LTC681xBus::BuildAddressedBusCommand(cmd, m_id));

  // Wait 15 ms for ADC to finish
  ThisThread::sleep_for(5ms); // TODO: This could be done differently

  uint8_t rxbuf[8 * 2]; 

  m_bus.SendReadCommand(LTC681xBus::BuildAddressedBusCommand(ReadAuxiliaryGroupA(), m_id), rxbuf);
  m_bus.SendReadCommand(LTC681xBus::BuildAddressedBusCommand(ReadAuxiliaryGroupB(), m_id), rxbuf + 8);

  uint16_t *voltages = new uint16_t[12]; //for LTC6811
  //uint16_t *voltages = new uint16_t[6];

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

uint16_t *LTC6811::getGpioPin(GpioSelection pin) {
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

void LTC6811::buildCOMMBytes(uint8_t icom, uint8_t fcom, uint8_t data, uint8_t *commBytes) {
  // COMMn (even byte): Upper 4 bits = ICOM, Lower 4 bits = upper half of data
  commBytes[0] = (icom << 4) | ((data >> 4) & 0x0F);
  
  // COMMn+1 (odd byte): Upper 4 bits = lower half of data, Lower 4 bits = FCOM
  commBytes[1] = ((data & 0x0F) << 4) | fcom;
}


float LTC6811::readTemperatureTMP1075(TMP1075_Handle_t *sensor) {
  uint8_t commData[6] = {0};
  uint8_t rxData[8] = {0}; 
  uint8_t tempBytes[2] = {0};

  // STEP 1: Write register pointer (tell sensor which register to read)
  // I2C Sequence: START -> [ADDR+W] -> ACK -> [REG] -> STOP
  // BYTE 0: Address with Write bit (ADDR << 1|0)
  buildCOMMBytes(0x6, 0x0, (sensor->i2c_address << 1) | 0x00, tempBytes);
  commData[0] = tempBytes[0];  // COMM0
  commData[1] = tempBytes[1];  // COMM1

  //BYTE 1: Register address (0x00 for temperature register)
  //FCOM 0X9 (Master NACK + STOP)
  buildCOMMBytes(0x1, 0x9, sensor->temp_reg, tempBytes);
  commData[2] = tempBytes[0];  // COMM2
  commData[3] = tempBytes[1];  // COMM3

  // BYTE 2: Dummy (not used)
  buildCOMMBytes(0x07, 0x0, 0x00, tempBytes);
  commData[4] = tempBytes[0];  // COMM4
  commData[5] = tempBytes[1];  // COMM5

  m_bus.WakeupBus();
  auto wrCmd = LTC681xBus::BuildAddressedBusCommand(WriteCommGroup(), m_id);
  m_bus.SendDataCommand(wrCmd, commData);

  ThisThread::sleep_for(10ms);
  
  m_bus.WakeupBus();
  auto stCmd = LTC681xBus::BuildAddressedBusCommand(StartComm(), m_id);

  static constexpr int num_bytes = 24/8*3;
  static uint8_t buf[6] = {0};

  m_bus.SendDataCommand(stCmd, buf);

  // ThisThread::sleep_for(3ms);
  //
  // // STEP 2: Read 2 bytes from temperature register
  // // I2C Sequence: START -> [ADDR+R] -> ACK -> [READ MSB] -> ACK -> [READ LSB] -> NACK+STOP
  // //BYTE 0: Address with Read bit (ADDR << 1 |1)
  // buildCOMMBytes(0x6, 0x0, (sensor->i2c_address << 1) | 0x01, tempBytes);
  // commData[0] = tempBytes[0];  // COMM0
  // commData[1] = tempBytes[1];  // COMM1
  //
  // //BYTE 1: Read MSB (master sends 0xFF as dummy, master ACKs)
  // buildCOMMBytes(0x0, 0x0, 0xFF, tempBytes);
  // commData[2] = tempBytes[0];  // COMM2
  // commData[3] = tempBytes[1];  // COMM3
  //
  // // BYTE 2: Read LSB (master sends 0xFF as dummy, master NACKs and STOP)
  // buildCOMMBytes(0x0, 0x9, 0xFF, tempBytes);
  // commData[4] = tempBytes[0];  // COMM4
  // commData[5] = tempBytes[1];  // COMM5
  //
  // m_bus.WakeupBus();
  // m_bus.SendDataCommand(wrCmd, commData);
  // m_bus.SendCommand(stCmd);
  //
  // ThisThread::sleep_for(3ms);
  //
  // auto rdCmd = LTC681xBus::BuildAddressedBusCommand(ReadCommGroup(), m_id);
  // m_bus.WakeupBus();
  // m_bus.SendReadCommand(rdCmd, rxData);
  //
  // // STEP 3: Extract Temperature Data from COMM register
  // // MSB is in D1 (rxData[2] upper nibble, rxData[3] lower nibble)
  // uint8_t tempMSB = ((rxData[2] & 0x0F) << 4) | ((rxData[3] >> 4) & 0x0F);
  //
  // // LSB is in D2 (rxData[4] upper nibble, rxData[5] lower nibble)
  // uint8_t tempLSB = ((rxData[4] & 0x0F) << 4) | ((rxData[5] >> 4) & 0x0F);
  //
  // // Combine MSB and LSB into 16-bit value
  // int16_t rawTemp = (tempMSB << 8) | tempLSB;
  //
  // // STEP 4: Convert to Celsius
  // // TMP1075-specific conversion:
  // // 12-bit temperature value stored in bits 15-4
  // // Each LSB = 0.0625°C
  // // float temperature = (rawTemp >> 4) * 0.0625f;
  //
  // return (rawTemp >> 4) * 0.0625f;
  return 0;
}

bool LTC6811::verifyI2CStatus(uint8_t *rxData) {
  uint8_t data1 = ((rxData[2] & 0x0F) << 4) | ((rxData[3] >> 4) & 0x0F);
  uint8_t data2 = ((rxData[4] & 0x0F) << 4) | ((rxData[5] >> 4) & 0x0F);
  
  if (data1 == 0xFF && data2 == 0xFF) {
    return false;  // No response from sensor
  }
  
  return true;
}

