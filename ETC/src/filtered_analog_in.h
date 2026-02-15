//
// Created by Jackson Pinsonneault on 10/28/25.
//

#ifndef MBED_OS_SMOOTHTOANALOGIN_H
#define MBED_OS_SMOOTHTOANALOGIN_H

#include <cstdint>
#include "mbed.h"

class FilteredAnalogIn {

public:
    /** Create a smoothed AnalogIn using EWMA (exponential weighted moving average)
     *
     * @param analog_pin Reference to an AnalogIn to use for reading
     * @param cutoff_frequency Cutoff frequency at the -3db level (see ../README.md, 𝜏 = 1 / (2π * f_c))
    */
    explicit FilteredAnalogIn(AnalogIn &analog_pin, const float cutoff_frequency) :
        _analog_pin(analog_pin) {
        _timer.start();
        set_time_constant(cutoff_frequency);
    }

    /** Read the referenced AnalogIn input voltage as an EWMA value, represented as a float in the range [0.0, 1.0]
     *
     * @returns A floating-point value representing the EWMA read, measured as a percentage
    */
    float read();

    /** Read the referenced AnalogIn input voltage as an EWMA value, represented as an unsigned short in the range [0x0, 0xFFFF]
     *
     * @returns 16-bit unsigned short representing the EWMA read, normalized to a 16-bit value
    */
    unsigned short read_u16();

    /** Read the referenced AnalogIn input voltage as an EWMA in volts. The output depends on the target board's
     * ADC reference voltage (typically equal to supply voltage). The ADC reference voltage
     * sets the maximum voltage the ADC can quantify (ie: ADC output == ADC_MAX_VALUE when Vin == Vref)
     *
     * The target's default ADC reference voltage is determined by the configuration
     * option target.default-adc_vref. The reference voltage for a particular input
     * can be manually specified by either the constructor or `AnalogIn::set_reference_voltage`.
     *
     * @returns A floating-point value representing the EWMA read, measured in volts.
    */
    float read_voltage();

    /** Sets the referenced AnalogIn's reference voltage.
     *
     * The AnalogIn's reference voltage is used to scale the output when calling AnalogIn::read_volts
     *
     * @param[in] vref New ADC reference voltage for the referenced AnalogIn.
    */
    void set_reference_voltage(float vref) const;

    /** Gets the referenced AnalogIn's reference voltage.
     *
     * @returns A floating-point value representing the referenced AnalogIn's reference voltage, measured in volts.
    */
    float get_reference_voltage() const;

    /** Changes the time constant to reflect the cutoff frequency at the -3db level
     *
     * @param cutoff_frequency Cutoff frequency at the -3db level (see ../README.md, 𝜏 = 1 / (2π * f_c))
    */
    void set_time_constant(float cutoff_frequency);

private:
    AnalogIn &_analog_pin;          //  Referenced AnalogIn which is used for reads
    float _time_constant;           //  Time constant for RC sampling (set using cutoff frequency at the -3db level)
    float _smoothed_value = 0;      //  The current value that should be returned by read()
    bool _is_initialized = false;   //  States whether the EWMA has started
    Timer _timer;                   //  Find the difference in times between reads
};

#endif //MBED_OS_SMOOTHTOANALOGIN_H