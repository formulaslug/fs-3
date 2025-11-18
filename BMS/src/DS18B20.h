#include "mbed.h"
#include "OneWire.h"

// For debugging only: search for onewire device addresses on the bus
// (useful for setup process)
void debug_search_for_ds18b20_address(OneWire& bus);

class DS18B20 {
public:
    explicit DS18B20(OneWire& onewire_bus, uint64_t device_address);

    // Begin temperature sense process
    void start_conversion(bool parasite_power_mode = false);

    // returns temperature in Degrees Celcius, multiplied by 2 (eg. 42 ==> 21C)
    uint8_t retrieve_conversion(bool type_s_sensor = false);

private:
    OneWire& bus;
    uint64_t address;
};
