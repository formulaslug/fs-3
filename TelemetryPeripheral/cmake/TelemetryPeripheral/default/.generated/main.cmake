include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(TelemetryPeripheral_default_library_list )

add_executable(${TelemetryPeripheral_default_image_name} ${TelemetryPeripheral_default_library_list})



# Add the link options from the rule file.
TelemetryPeripheral_default_link_rule(${TelemetryPeripheral_default_image_name})


# Post build target to copy built file to the output directory.
add_custom_command(TARGET ${TelemetryPeripheral_default_image_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${TelemetryPeripheral_default_output_dir}
                    COMMAND ${CMAKE_COMMAND} -E copy ${TelemetryPeripheral_default_image_name} ${TelemetryPeripheral_default_output_dir}/${TelemetryPeripheral_default_original_image_name}
                    BYPRODUCTS ${TelemetryPeripheral_default_output_dir}/${TelemetryPeripheral_default_original_image_name})
