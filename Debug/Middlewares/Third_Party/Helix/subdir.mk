################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/Helix/mp3dec.c \
../Middlewares/Third_Party/Helix/mp3tabs.c 

OBJS += \
./Middlewares/Third_Party/Helix/mp3dec.o \
./Middlewares/Third_Party/Helix/mp3tabs.o 

C_DEPS += \
./Middlewares/Third_Party/Helix/mp3dec.d \
./Middlewares/Third_Party/Helix/mp3tabs.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/Helix/mp3dec.o: ../Middlewares/Third_Party/Helix/mp3dec.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/mp3dec.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/mp3tabs.o: ../Middlewares/Third_Party/Helix/mp3tabs.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/mp3tabs.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

