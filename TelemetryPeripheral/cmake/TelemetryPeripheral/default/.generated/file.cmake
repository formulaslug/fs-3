# The following variables contains the files used by the different stages of the build process.

# The (internal) path to the resulting build image.
set(TelemetryPeripheral_default_internal_image_name "${CMAKE_CURRENT_SOURCE_DIR}/../../../_build/TelemetryPeripheral/default/default.elf")

# The name of the resulting image, including namespace for configuration.
set(TelemetryPeripheral_default_image_name "TelemetryPeripheral_default_default.elf")

# The name of the image, excluding the namespace for configuration.
set(TelemetryPeripheral_default_original_image_name "default.elf")

# The output directory of the final image.
set(TelemetryPeripheral_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/TelemetryPeripheral")
