# The following variables contains the files used by the different stages of the build process.
set(TelemetryPeripheral_default_default_AVR_GCC_FILE_TYPE_assemble)
set(TelemetryPeripheral_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/src/protected_io.S")
set_source_files_properties(${TelemetryPeripheral_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess} PROPERTIES LANGUAGE C)
set(TelemetryPeripheral_default_default_AVR_GCC_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../d6t-8lh.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/delay.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/device_config.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/drivers/i2c_simple_master.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/drivers/spi_master.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/examples/twi0_master_example.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/mcc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/src/adc0.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/src/adc1.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/src/cpuint.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/src/pin_manager.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/src/spi0.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../mcc_generated_files/src/twi0_master.c")
set(TelemetryPeripheral_default_default_AVR_GCC_FILE_TYPE_link)

# The (internal) path to the resulting build image.
set(TelemetryPeripheral_default_internal_image_name "${CMAKE_CURRENT_SOURCE_DIR}/../../../_build/TelemetryPeripheral/default/default.elf")

# The name of the resulting image, including namespace for configuration.
set(TelemetryPeripheral_default_image_name "TelemetryPeripheral_default_default.elf")

# The name of the image, excluding the namespace for configuration.
set(TelemetryPeripheral_default_original_image_name "default.elf")

# The output directory of the final image.
set(TelemetryPeripheral_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/TelemetryPeripheral")
