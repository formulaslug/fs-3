include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(TelemetryPeripheral_default_library_list )

# Handle files with suffix s, for group default-AVR-GCC
if(TelemetryPeripheral_default_default_AVR_GCC_FILE_TYPE_assemble)
add_library(TelemetryPeripheral_default_default_AVR_GCC_assemble OBJECT ${TelemetryPeripheral_default_default_AVR_GCC_FILE_TYPE_assemble})
    TelemetryPeripheral_default_default_AVR_GCC_assemble_rule(TelemetryPeripheral_default_default_AVR_GCC_assemble)
    list(APPEND TelemetryPeripheral_default_library_list "$<TARGET_OBJECTS:TelemetryPeripheral_default_default_AVR_GCC_assemble>")
endif()

# Handle files with suffix S, for group default-AVR-GCC
if(TelemetryPeripheral_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess)
add_library(TelemetryPeripheral_default_default_AVR_GCC_assembleWithPreprocess OBJECT ${TelemetryPeripheral_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess})
    TelemetryPeripheral_default_default_AVR_GCC_assembleWithPreprocess_rule(TelemetryPeripheral_default_default_AVR_GCC_assembleWithPreprocess)
    list(APPEND TelemetryPeripheral_default_library_list "$<TARGET_OBJECTS:TelemetryPeripheral_default_default_AVR_GCC_assembleWithPreprocess>")
endif()

# Handle files with suffix [cC], for group default-AVR-GCC
if(TelemetryPeripheral_default_default_AVR_GCC_FILE_TYPE_compile)
add_library(TelemetryPeripheral_default_default_AVR_GCC_compile OBJECT ${TelemetryPeripheral_default_default_AVR_GCC_FILE_TYPE_compile})
    TelemetryPeripheral_default_default_AVR_GCC_compile_rule(TelemetryPeripheral_default_default_AVR_GCC_compile)
    list(APPEND TelemetryPeripheral_default_library_list "$<TARGET_OBJECTS:TelemetryPeripheral_default_default_AVR_GCC_compile>")
endif()

add_executable(${TelemetryPeripheral_default_image_name} ${TelemetryPeripheral_default_library_list})

target_link_libraries(${TelemetryPeripheral_default_image_name} PRIVATE ${TelemetryPeripheral_default_default_AVR_GCC_FILE_TYPE_link})

# Add the link options from the rule file.
TelemetryPeripheral_default_link_rule(${TelemetryPeripheral_default_image_name})


# Post build target to copy built file to the output directory.
add_custom_command(TARGET ${TelemetryPeripheral_default_image_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${TelemetryPeripheral_default_output_dir}
                    COMMAND ${CMAKE_COMMAND} -E copy ${TelemetryPeripheral_default_image_name} ${TelemetryPeripheral_default_output_dir}/${TelemetryPeripheral_default_original_image_name}
                    BYPRODUCTS ${TelemetryPeripheral_default_output_dir}/${TelemetryPeripheral_default_original_image_name})
