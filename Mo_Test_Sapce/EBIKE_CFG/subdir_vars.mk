################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../ebike_memory_flash_linker.cmd 

SYSCFG_SRCS += \
../ebike.syscfg 

LIB_SRCS += \
C:/ti/c2000/C2000Ware_4_02_00_00/driverlib/f28004x/driverlib/ccs/Debug/driverlib.lib 

C_SRCS += \
../RiderStats.c \
./syscfg/board.c \
./syscfg/c2000ware_libraries.c \
../empty_driverlib_main.c 

GEN_FILES += \
./syscfg/board.c \
./syscfg/c2000ware_libraries.opt \
./syscfg/c2000ware_libraries.c 

GEN_MISC_DIRS += \
./syscfg/ 

C_DEPS += \
./RiderStats.d \
./syscfg/board.d \
./syscfg/c2000ware_libraries.d \
./empty_driverlib_main.d 

GEN_OPTS += \
./syscfg/c2000ware_libraries.opt 

OBJS += \
./RiderStats.obj \
./syscfg/board.obj \
./syscfg/c2000ware_libraries.obj \
./empty_driverlib_main.obj 

GEN_MISC_FILES += \
./syscfg/board.h \
./syscfg/pinmux.csv \
./syscfg/c2000ware_libraries.cmd.genlibs \
./syscfg/c2000ware_libraries.h 

GEN_MISC_DIRS__QUOTED += \
"syscfg\" 

OBJS__QUOTED += \
"RiderStats.obj" \
"syscfg\board.obj" \
"syscfg\c2000ware_libraries.obj" \
"empty_driverlib_main.obj" 

GEN_MISC_FILES__QUOTED += \
"syscfg\board.h" \
"syscfg\pinmux.csv" \
"syscfg\c2000ware_libraries.cmd.genlibs" \
"syscfg\c2000ware_libraries.h" 

C_DEPS__QUOTED += \
"RiderStats.d" \
"syscfg\board.d" \
"syscfg\c2000ware_libraries.d" \
"empty_driverlib_main.d" 

GEN_FILES__QUOTED += \
"syscfg\board.c" \
"syscfg\c2000ware_libraries.opt" \
"syscfg\c2000ware_libraries.c" 

C_SRCS__QUOTED += \
"../RiderStats.c" \
"./syscfg/board.c" \
"./syscfg/c2000ware_libraries.c" \
"../empty_driverlib_main.c" 

SYSCFG_SRCS__QUOTED += \
"../ebike.syscfg" 


