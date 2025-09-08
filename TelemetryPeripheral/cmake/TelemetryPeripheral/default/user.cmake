# Make AVR-LibC work with the attiny1616.
# From device-specs/specs-attiny1616 of the avr-gcc toolchain:
# /*
# * AVR-LibC's avr/io.h uses the device specifying macro to determine
# * the name of the device header.  For example, -mmcu=atmega8a triggers
# * the definition of __AVR_ATmega8A__ and avr/io.h includes the device
# * header 'iom8a.h' by means of:
# *
# *     ...
# *     #elif defined (__AVR_ATmega8A__)
# *     #  include <avr/iom8a.h>
# *     #elif ...
# * 
# * If no device macro is defined, AVR-LibC uses __AVR_DEV_LIB_NAME__
# * as fallback to determine the name of the device header as
# *
# *     "avr/io" + __AVR_DEV_LIB_NAME__ + ".h"
# *
# * If you provide your own specs file for a device not yet known to
# * AVR-LibC, you can now define the hook macro __AVR_DEV_LIB_NAME__
# * as needed so that
# *
# *     #include <avr/io.h>
# *
# * will include the desired device header.  For ATmega8A the supplement
# * to *cpp_avrlibc would read
# *
# *     -D__AVR_DEV_LIB_NAME__=m8a
# */
# So, this makes `#include <avr/io.h> work :)
add_definitions(
    -D__AVR_DEV_LIB_NAME__=tn1616
    -D__AVR_DEVICE_NAME__=attiny1616
)
