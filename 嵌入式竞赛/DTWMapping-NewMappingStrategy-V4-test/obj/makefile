################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
DTWMapping-NewMappingStrategy-V4-test.hex \

SECONDARY_LIST += \
DTWMapping-NewMappingStrategy-V4-test.lst \

SECONDARY_SIZE += \
DTWMapping-NewMappingStrategy-V4-test.siz \


# All Target
all: DTWMapping-NewMappingStrategy-V4-test.elf secondary-outputs

# Tool invocations
DTWMapping-NewMappingStrategy-V4-test.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized  -g -T "C:\Users\wysji\Desktop\DTWMapping-NewMappingStrategy-V4-test\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"DTWMapping-NewMappingStrategy-V4-test.map" --specs=nano.specs --specs=nosys.specs -o "DTWMapping-NewMappingStrategy-V4-test.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
DTWMapping-NewMappingStrategy-V4-test.hex: DTWMapping-NewMappingStrategy-V4-test.elf
	@	riscv-none-embed-objcopy -O ihex "DTWMapping-NewMappingStrategy-V4-test.elf"  "DTWMapping-NewMappingStrategy-V4-test.hex"
	@	@
DTWMapping-NewMappingStrategy-V4-test.lst: DTWMapping-NewMappingStrategy-V4-test.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble "DTWMapping-NewMappingStrategy-V4-test.elf" > "DTWMapping-NewMappingStrategy-V4-test.lst"
	@	@
DTWMapping-NewMappingStrategy-V4-test.siz: DTWMapping-NewMappingStrategy-V4-test.elf
	@	riscv-none-embed-size --format=berkeley "DTWMapping-NewMappingStrategy-V4-test.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) DTWMapping-NewMappingStrategy-V4-test.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
