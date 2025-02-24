# WARNING:
# This makefile has absolutely nothing to do with the build process.
# This is JUST used as a post-build script runner.

# makefile resource that includes compiling manually:
# https://gist.github.com/edwardhotchkiss/9378977

OUT_DIRECTORY = out/TelemetryPeripheral

.PHONY: *

default: all

all: hex upload

# todo: is this working correctly? Seems like the .hex is waaay smaller than the .elf
hex:
	avr-objcopy -O ihex $(OUT_DIRECTORY)/default.elf $(OUT_DIRECTORY)/default.hex

# not implemented yet, but will use the `-c serialupdi` programmer:
# https://avrdudes.github.io/avrdude/8.0/avrdude_22.html#SerialUPDI-Programmer
# (alternatively, we can also use pymcuprog instead of avrdude)
upload:
	avrdude 

link_compile_commands:
	ln -s _build/TelemetryPeripheral/default/compile_commands.json .