# WARNING:
# This makefile has absolutely nothing to do with the build process.
# This is JUST used as a post-build script runner.

# makefile resource that includes compiling manually:
# https://gist.github.com/edwardhotchkiss/9378977

OUT_DIRECTORY = out/TelemetryPeripheral
BUILD_DIRECTORY = _build/TelemetryPeripheral/default

.PHONY: *

default: all

all: build hex upload

build:
	ninja -C $(BUILD_DIRECTORY)

# todo: is this working correctly? Seems like the .hex is waaay smaller than the .elf
hex:
	avr-objcopy -O ihex $(OUT_DIRECTORY)/default.elf $(OUT_DIRECTORY)/default.hex

upload:
	avrdude -c serialupdi -p attiny1616 -P /dev/ttyUSB0 -e -U flash:w:out/TelemetryPeripheral/default.elf

link_compile_commands:
	ln -s $(BUILD_DIRECTORY)/compile_commands.json .
