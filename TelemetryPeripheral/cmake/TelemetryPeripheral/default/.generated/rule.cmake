# The following functions contains all the flags passed to the different build stages.

set(PACK_REPO_PATH "C:/Users/Goob/.mchp_packs" CACHE PATH "Path to the root of a pack repository.")

function(TelemetryPeripheral_default_default_AVR_GCC_assemble_rule target)
    set(options
        "-x"
        "assembler-with-cpp"
        "${MP_EXTRA_AS_PRE}"
        "-B${PACK_REPO_PATH}/Microchip/ATtiny_DFP/3.2.268/gcc/dev/attiny1616"
        "-c"
        "-Wa,--defsym=__MPLAB_BUILD=1${MP_EXTRA_AS_POST}")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__ATtiny1616__"
        PRIVATE "default=default")
    target_include_directories(${target} PRIVATE "${PACK_REPO_PATH}/Microchip/ATtiny_DFP/3.2.268/include")
endfunction()
function(TelemetryPeripheral_default_default_AVR_GCC_assembleWithPreprocess_rule target)
    set(options
        "-x"
        "assembler-with-cpp"
        "${MP_EXTRA_AS_PRE}"
        "-mmcu=attiny1616"
        "-B${PACK_REPO_PATH}/Microchip/ATtiny_DFP/3.2.268/gcc/dev/attiny1616"
        "-c"
        "-Wa,--defsym=__MPLAB_BUILD=1${MP_EXTRA_AS_POST}")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__ATtiny1616__"
        PRIVATE "default=default")
    target_include_directories(${target} PRIVATE "${PACK_REPO_PATH}/Microchip/ATtiny_DFP/3.2.268/include")
endfunction()
function(TelemetryPeripheral_default_default_AVR_GCC_compile_rule target)
    set(options
        "-g"
        "-gdwarf-2"
        "${MP_EXTRA_CC_PRE}"
        "-mmcu=attiny1616"
        "-B${PACK_REPO_PATH}/Microchip/ATtiny_DFP/3.2.268/gcc/dev/attiny1616"
        "-x"
        "c"
        "-c"
        "-funsigned-char"
        "-funsigned-bitfields"
        "-O1"
        "-ffunction-sections"
        "-fdata-sections"
        "-fpack-struct"
        "-fshort-enums"
        "-Wall")
    list(REMOVE_ITEM options "")
    target_compile_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__ATtiny1616__"
        PRIVATE "default=default")
    target_include_directories(${target} PRIVATE "${PACK_REPO_PATH}/Microchip/ATtiny_DFP/3.2.268/include")
endfunction()
function(TelemetryPeripheral_default_link_rule target)
    set(options
        "-gdwarf-2"
        "${MP_EXTRA_LD_PRE}"
        "-mmcu=attiny1616"
        "-B${PACK_REPO_PATH}/Microchip/ATtiny_DFP/3.2.268/gcc/dev/attiny1616"
        "-o"
        "${FINAL_IMAGE_NAME}"
        "-Wl,--defsym=__MPLAB_BUILD=1${MP_EXTRA_LD_POST}"
        "-Wl,--gc-sections")
    list(REMOVE_ITEM options "")
    target_link_options(${target} PRIVATE "${options}")
    target_compile_definitions(${target}
        PRIVATE "__ATtiny1616__"
        PRIVATE "default=default")
endfunction()
