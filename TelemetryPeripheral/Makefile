# WARNING:
# This makefile has absolutely nothing to do with the build process.
# This is JUST used as a post-build script runner.

# makefile resource that includes compiling manually:
# https://gist.github.com/edwardhotchkiss/9378977

OUT_DIRECTORY = out/TelemetryPeripheral
BUILD_DIRECTORY = _build/TelemetryPeripheral/default
CMAKE_DIRECTORY = cmake/TelemetryPeripheral/default

# Require avrdude and the avr toolchain (avr-gcc) to be present in PATH
ifeq (, $(shell which avrdude))
$(error "No avrdude found in PATH!")
endif
ifeq (, $(shell which avr-gcc))
$(error "No avr toolchain found in PATH! \(avr-gcc\)")
endif

.PHONY: *

default:
	$(error "Must run one of make br/bl/fr/fl!")
fr:
	$(MAKE) WHEEL_POSITION=0 build upload
fl:
	$(MAKE) WHEEL_POSITION=1 build upload
br:
	$(MAKE) WHEEL_POSITION=2 build upload
bl:
	$(MAKE) WHEEL_POSITION=3 build upload

build: configure
	ninja -C build

configure:
	cmake -S $(CMAKE_DIRECTORY) -B $(BUILD_DIRECTORY) -GNinja -DWHEEL_POSITION=$(WHEEL_POSITION)

build:
	ninja -C $(BUILD_DIRECTORY)

# /dev/ttyACM0 /dev/ttyUSB0
upload:
	avrdude -c serialupdi -p attiny1616 -P ch9102 -e -U flash:w:out/TelemetryPeripheral/default.elf # adafruit UPDI friend programmer
	# avrdude -c serialupdi -p attiny1616 -P pl2303 -e -U flash:w:out/TelemetryPeripheral/default.elf # adafruit USB -TTL serial adapter

link_compile_commands:
	ln -s $(BUILD_DIRECTORY)/compile_commands.json .


