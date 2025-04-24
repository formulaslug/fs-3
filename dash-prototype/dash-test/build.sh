#! /bin/zsh

rm -rf build

cmake . --preset Debug

cmake --build --preset Debug

arm-none-eabi-objcopy -O binary build/Debug/dash-test.elf build/Debug/dash-test.bin

#st-flash erase

#st-flash write build/Debug/dash-test.bin 0x8000000

