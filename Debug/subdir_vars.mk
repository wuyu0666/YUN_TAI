################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
SYSCFG_SRCS += \
../empty.syscfg 

C_SRCS += \
../bihuan_pid.c \
../bujin.c \
../bujin2.c \
../delay.c \
./ti_msp_dl_config.c \
C:/ti/mspm0_sdk_2_10_00_04/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c \
../key.c \
../key2.c \
../main.c \
../oled.c \
../uart.c 

GEN_CMDS += \
./device_linker.cmd 

GEN_FILES += \
./device_linker.cmd \
./device.opt \
./ti_msp_dl_config.c 

C_DEPS += \
./bihuan_pid.d \
./bujin.d \
./bujin2.d \
./delay.d \
./ti_msp_dl_config.d \
./startup_mspm0g350x_ticlang.d \
./key.d \
./key2.d \
./main.d \
./oled.d \
./uart.d 

GEN_OPTS += \
./device.opt 

OBJS += \
./bihuan_pid.o \
./bujin.o \
./bujin2.o \
./delay.o \
./ti_msp_dl_config.o \
./startup_mspm0g350x_ticlang.o \
./key.o \
./key2.o \
./main.o \
./oled.o \
./uart.o 

GEN_MISC_FILES += \
./device.cmd.genlibs \
./ti_msp_dl_config.h \
./Event.dot 

OBJS__QUOTED += \
"bihuan_pid.o" \
"bujin.o" \
"bujin2.o" \
"delay.o" \
"ti_msp_dl_config.o" \
"startup_mspm0g350x_ticlang.o" \
"key.o" \
"key2.o" \
"main.o" \
"oled.o" \
"uart.o" 

GEN_MISC_FILES__QUOTED += \
"device.cmd.genlibs" \
"ti_msp_dl_config.h" \
"Event.dot" 

C_DEPS__QUOTED += \
"bihuan_pid.d" \
"bujin.d" \
"bujin2.d" \
"delay.d" \
"ti_msp_dl_config.d" \
"startup_mspm0g350x_ticlang.d" \
"key.d" \
"key2.d" \
"main.d" \
"oled.d" \
"uart.d" 

GEN_FILES__QUOTED += \
"device_linker.cmd" \
"device.opt" \
"ti_msp_dl_config.c" 

C_SRCS__QUOTED += \
"../bihuan_pid.c" \
"../bujin.c" \
"../bujin2.c" \
"../delay.c" \
"./ti_msp_dl_config.c" \
"C:/ti/mspm0_sdk_2_10_00_04/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c" \
"../key.c" \
"../key2.c" \
"../main.c" \
"../oled.c" \
"../uart.c" 

SYSCFG_SRCS__QUOTED += \
"../empty.syscfg" 


