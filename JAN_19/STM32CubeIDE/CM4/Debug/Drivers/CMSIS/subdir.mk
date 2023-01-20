################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/STM32CubeIDE/Disco_Firmware_Sem2/JAN_19/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c 

C_DEPS += \
./Drivers/CMSIS/system_stm32h7xx_dualcore_boot_cm4_cm7.d 

OBJS += \
./Drivers/CMSIS/system_stm32h7xx_dualcore_boot_cm4_cm7.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/system_stm32h7xx_dualcore_boot_cm4_cm7.o: D:/STM32CubeIDE/Disco_Firmware_Sem2/JAN_19/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c Drivers/CMSIS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H745xx -c -I../../../CM4/Core/Inc -I../../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-CMSIS

clean-Drivers-2f-CMSIS:
	-$(RM) ./Drivers/CMSIS/system_stm32h7xx_dualcore_boot_cm4_cm7.d ./Drivers/CMSIS/system_stm32h7xx_dualcore_boot_cm4_cm7.o ./Drivers/CMSIS/system_stm32h7xx_dualcore_boot_cm4_cm7.su

.PHONY: clean-Drivers-2f-CMSIS

