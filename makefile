SHELL := /bin/bash
.DEFAULT_GOAL := all
.SUFFIXES:
MAKEFLAGS += --no-builtin-rules

TARGET_EXEC ?= stm_target
BUILD_DIRECTORY ?= build
BUILD_TYPE ?= release
ENABLE_LTO ?= 0
WERROR ?= 0

ARM_VERSION_TOOLCHAIN ?= gcc-arm-none-eabi-10.3-2021.10
TOOLCHAIN_DIRECTORY ?= /home/abdelrahman/tools/$(ARM_VERSION_TOOLCHAIN)/bin
TOOLCHAIN_PREFIX ?= $(TOOLCHAIN_DIRECTORY)/arm-none-eabi-

CC := $(TOOLCHAIN_PREFIX)gcc
CXX := $(TOOLCHAIN_PREFIX)g++
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy
SIZE := $(TOOLCHAIN_PREFIX)size

LINKER_SCRIPT ?= Linkerscript/STM32F411.ld
MCU_DEFINES ?= STM32F411xE USE_STDPERIPH_DRIVER

ifeq ($(wildcard $(LINKER_SCRIPT)),)
$(error Missing linker script: $(LINKER_SCRIPT))
endif

REQUIRED_TOOLS := $(CC) $(CXX) $(OBJCOPY) $(SIZE)
$(foreach tool,$(REQUIRED_TOOLS),$(if $(wildcard $(tool)),,$(error Missing toolchain binary: $(tool))))

TARGET_ELF := $(BUILD_DIRECTORY)/$(TARGET_EXEC).elf
TARGET_HEX := $(BUILD_DIRECTORY)/$(TARGET_EXEC).hex
TARGET_BIN := $(BUILD_DIRECTORY)/$(TARGET_EXEC).bin
TARGET_MAP := $(BUILD_DIRECTORY)/$(TARGET_EXEC).map

SRC_FILES := Main.cpp
SRC_FILES += Source/Hardware/ClockDriver/ClockDriver.cpp
SRC_FILES += startup.c
SRC_FILES += STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c
SRC_FILES += STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c
SRC_FILES += STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
SRC_FILES += STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c
SRC_FILES += STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
SRC_FILES += STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c
SRC_FILES += STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c
SRC_FILES += FreeRTOS/FreeRTOS/Source/list.c
SRC_FILES += FreeRTOS/FreeRTOS/Source/tasks.c
SRC_FILES += FreeRTOS/FreeRTOS/Source/queue.c
SRC_FILES += FreeRTOS/FreeRTOS/Source/timers.c
SRC_FILES += FreeRTOS/FreeRTOS/Source/event_groups.c
SRC_FILES += FreeRTOS/FreeRTOS/Source/stream_buffer.c
SRC_FILES += FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
SRC_FILES += FreeRTOS/FreeRTOS/Source/portable/MemMang/heap_4.c


PROJECT_INCLUDE_DIRECTORIES := .
PROJECT_INCLUDE_DIRECTORIES += Source/Hardware/ClockDriver
PROJECT_INCLUDE_DIRECTORIES += Source/Hardware/IWDGDriver
PROJECT_INCLUDE_DIRECTORIES += Source/Hardware
PROJECT_INCLUDE_DIRECTORIES += Source/Hardware/GPIOConfigurationDriver
PROJECT_INCLUDE_DIRECTORIES += Source/Hardware/UARTDriver
PROJECT_INCLUDE_DIRECTORIES += Source/Software/Sensor
PROJECT_INCLUDE_DIRECTORIES += Source/Software/LCD
PROJECT_INCLUDE_DIRECTORIES += FreeRTOS/FreeRTOS/Source/include
PROJECT_INCLUDE_DIRECTORIES += FreeRTOS/FreeRTOS/Source/portable/GCC/ARM_CM4F

THIRD_PARTY_INCLUDE_DIRECTORIES := STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/CMSIS/Include
THIRD_PARTY_INCLUDE_DIRECTORIES += STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/CMSIS/Device/ST/STM32F4xx/Include
THIRD_PARTY_INCLUDE_DIRECTORIES += STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/inc

CPU_FLAGS := -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
SECTION_FLAGS := -ffunction-sections -fdata-sections
WARNING_FLAGS := -Wall -Wextra -Wshadow -Wformat=2

CPPFLAGS := $(addprefix -I,$(PROJECT_INCLUDE_DIRECTORIES)) \
	$(addprefix -isystem ,$(THIRD_PARTY_INCLUDE_DIRECTORIES)) \
	$(addprefix -D,$(MCU_DEFINES)) -MMD -MP
CFLAGS := $(CPU_FLAGS) $(SECTION_FLAGS) $(WARNING_FLAGS) -std=gnu11
CXXFLAGS := $(CPU_FLAGS) $(SECTION_FLAGS) $(WARNING_FLAGS) -std=gnu++14 -fno-exceptions -fno-rtti -fno-use-cxa-atexit
LDFLAGS := $(CPU_FLAGS) -T$(LINKER_SCRIPT) --specs=nano.specs --specs=nosys.specs \
	-Wl,--gc-sections -Wl,--print-memory-usage -Wl,-Map,$(TARGET_MAP)
LDLIBS := -lm -lc -lnosys

ifeq ($(WERROR),1)
	CFLAGS += -Werror
	CXXFLAGS += -Werror
endif

ifeq ($(BUILD_TYPE),debug)
	CFLAGS += -Og -g3
	CXXFLAGS += -Og -g3
else ifeq ($(BUILD_TYPE),release)
	CFLAGS += -O2 -g0
	CXXFLAGS += -O2 -g0
else
$(error Unsupported BUILD_TYPE='$(BUILD_TYPE)'. Use 'debug' or 'release')
endif

ifeq ($(ENABLE_LTO),1)
	CFLAGS += -flto
	CXXFLAGS += -flto
	LDFLAGS += -flto
endif

C_SRCS := $(filter %.c,$(SRC_FILES))
CPP_SRCS := $(filter %.cpp,$(SRC_FILES))
C_OBJS := $(addprefix $(BUILD_DIRECTORY)/,$(C_SRCS:.c=.o))
CPP_OBJS := $(addprefix $(BUILD_DIRECTORY)/,$(CPP_SRCS:.cpp=.o))
OBJS := $(C_OBJS) $(CPP_OBJS)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean stm_target size print-config

all: $(TARGET_ELF) $(TARGET_HEX) $(TARGET_BIN)
stm_target: all

$(TARGET_ELF): $(OBJS) | $(BUILD_DIRECTORY)
	$(CXX) $(OBJS) $(LDFLAGS) -Wl,--start-group $(LDLIBS) -Wl,--end-group -o $@
	$(SIZE) $@

$(TARGET_HEX): $(TARGET_ELF)
	$(OBJCOPY) -O ihex $< $@

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIRECTORY)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIRECTORY)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIRECTORY):
	mkdir -p $@

size: $(TARGET_ELF)
	$(SIZE) $(TARGET_ELF)

print-config:
	@echo "TARGET_EXEC: $(TARGET_EXEC)"
	@echo "BUILD_TYPE: $(BUILD_TYPE)"
	@echo "TOOLCHAIN_DIRECTORY: $(TOOLCHAIN_DIRECTORY)"
	@echo "LINKER_SCRIPT: $(LINKER_SCRIPT)"

clean:
	rm -rf $(BUILD_DIRECTORY)

-include $(DEPS)
