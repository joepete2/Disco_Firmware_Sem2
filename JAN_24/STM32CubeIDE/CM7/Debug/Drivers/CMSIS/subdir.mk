################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/joepe/STM32CubeIDE/workspace_1.11.0_2/JAN_24/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c 

OBJS += \
./Drivers/CMSIS/system_stm32h7xx_dualcore_boot_cm4_cm7.o 

C_DEPS += \
./Drivers/CMSIS/system_stm32h7xx_dualcore_boot_cm4_cm7.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/system_stm32h7xx_dualcore_boot_cm4_cm7.o: C:/Users/joepe/STM32CubeIDE/workspace_1.11.0_2/JAN_24/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c Drivers/CMSIS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H745xx -c -I../../../CM7/Core/Inc -I../../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-CMSIS

clean-Drivers-2f-CMSIS:
	-$(RM) ./Drivers/CMSIS/system_stm32h7xx_dualcore_boot_cm4_cm7.d ./Drivers/CMSIS/system_stm32h7xx_dualcore_boot_cm4_cm7.o ./Drivers/CMSIS/system_stm32h7xx_dualcore_boot_cm4_cm7.su

.PHONY: clean-Drivers-2f-CMSIS

