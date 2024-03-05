cmake_minimum_required(VERSION 3.20)

set(SETTINGS_PROGRAMMER usbasp)
set(SETTINGS_MCU        atmega328p)
set(SETTINGS_F_CPU      1000000)
set(SETTINGS_BAUD       9600)

set(PROGRAM_AVRCPP   "avr-g++")
set(PROGRAM_AVRC     "avr-gcc")
set(PROGRAM_AVRSTRIP "avr-strip")
set(PROGRAM_OBJCOPY  "avr-objcopy")
set(PROGRAM_OBJDUMP  "avr-objdump")
set(PROGRAM_AVRSIZE  "avr-size")
set(PROGRAM_AVRDUDE  "avrdude")

add_definitions(-DF_CPU=${SETTINGS_F_CPU}UL)
add_definitions(-DBAUD=${SETTINGS_BAUD})

if (ATMEGA_COMPILER AND NOT UNIX)
    message(FATAL_ERROR "ATMEGA_COMPILER requires Unix OS, run it under docker or Windows Subsystem for Linux")
endif ()

project(Mk2 C ASM)

if (ATMEGA_COMPILER)
    add_definitions(-DATMEGA_COMPILER)

    set(CMAKE_C_FLAGS "")
    set(CMAKE_C_FLAGS_INIT "")
    set(CMAKE_C_FLAGS_RELEASE "")
    set(CMAKE_C_FLAGS_RELEASE_INIT "")

    set(CMCU         "-mmcu=${SETTINGS_MCU}")
    set(CDEBUG       "-gstabs -g -ggdb")
    set(CDEFS        "-DATMEGA_COMPILER")
    set(CLINKS       "-lm")
    set(COPT         "-Os")
    set(CWARN        "-Wall -Wextra -pedantic -Wstrict-prototypes -Wl,--gc-sections")
    set(CSTANDARD    "-std=gnu11")
    set(CTUNING      "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections")

    set(CMAKE_C_FLAGS "${CMCU} ${CDEBUG} ${CDEFS} ${CLINKS} ${COPT} ${CWARN} ${CSTANDARD} ${CTUNING} ")
endif (ATMEGA_COMPILER)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")


add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


if (ATMEGA_COMPILER)
    # Set up compiling targets
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")
    add_custom_target(strip ALL  ${PROGRAM_AVRSTRIP} "${PROJECT_NAME}.elf" DEPENDS ${PROJECT_NAME})
    add_custom_target(hex   ALL  ${PROGRAM_OBJCOPY} -R .eeprom -O ihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex" DEPENDS strip)
    add_custom_target(eeprom     ${PROGRAM_OBJCOPY} -j .eeprom --change-section-lma .eeprom=0 -O ihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.eeprom" DEPENDS strip)
    add_custom_target(flash ${PROGRAM_AVRDUDE} -c ${PROGRAMMER_TYPE} -p ${MCU} -U flash:w:${PROJECT_NAME}.hex DEPENDS hex)
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.lst")
endif (ATMEGA_COMPILER)
