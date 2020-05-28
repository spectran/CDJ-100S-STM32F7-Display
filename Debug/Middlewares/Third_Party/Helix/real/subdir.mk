################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/Helix/real/bitstream.c \
../Middlewares/Third_Party/Helix/real/buffers.c \
../Middlewares/Third_Party/Helix/real/dct32.c \
../Middlewares/Third_Party/Helix/real/dequant.c \
../Middlewares/Third_Party/Helix/real/dqchan.c \
../Middlewares/Third_Party/Helix/real/huffman.c \
../Middlewares/Third_Party/Helix/real/hufftabs.c \
../Middlewares/Third_Party/Helix/real/imdct.c \
../Middlewares/Third_Party/Helix/real/polyphase.c \
../Middlewares/Third_Party/Helix/real/scalfact.c \
../Middlewares/Third_Party/Helix/real/stproc.c \
../Middlewares/Third_Party/Helix/real/subband.c \
../Middlewares/Third_Party/Helix/real/trigtabs_fixpt.c 

OBJS += \
./Middlewares/Third_Party/Helix/real/bitstream.o \
./Middlewares/Third_Party/Helix/real/buffers.o \
./Middlewares/Third_Party/Helix/real/dct32.o \
./Middlewares/Third_Party/Helix/real/dequant.o \
./Middlewares/Third_Party/Helix/real/dqchan.o \
./Middlewares/Third_Party/Helix/real/huffman.o \
./Middlewares/Third_Party/Helix/real/hufftabs.o \
./Middlewares/Third_Party/Helix/real/imdct.o \
./Middlewares/Third_Party/Helix/real/polyphase.o \
./Middlewares/Third_Party/Helix/real/scalfact.o \
./Middlewares/Third_Party/Helix/real/stproc.o \
./Middlewares/Third_Party/Helix/real/subband.o \
./Middlewares/Third_Party/Helix/real/trigtabs_fixpt.o 

C_DEPS += \
./Middlewares/Third_Party/Helix/real/bitstream.d \
./Middlewares/Third_Party/Helix/real/buffers.d \
./Middlewares/Third_Party/Helix/real/dct32.d \
./Middlewares/Third_Party/Helix/real/dequant.d \
./Middlewares/Third_Party/Helix/real/dqchan.d \
./Middlewares/Third_Party/Helix/real/huffman.d \
./Middlewares/Third_Party/Helix/real/hufftabs.d \
./Middlewares/Third_Party/Helix/real/imdct.d \
./Middlewares/Third_Party/Helix/real/polyphase.d \
./Middlewares/Third_Party/Helix/real/scalfact.d \
./Middlewares/Third_Party/Helix/real/stproc.d \
./Middlewares/Third_Party/Helix/real/subband.d \
./Middlewares/Third_Party/Helix/real/trigtabs_fixpt.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/Helix/real/bitstream.o: ../Middlewares/Third_Party/Helix/real/bitstream.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/bitstream.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/real/buffers.o: ../Middlewares/Third_Party/Helix/real/buffers.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/buffers.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/real/dct32.o: ../Middlewares/Third_Party/Helix/real/dct32.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/dct32.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/real/dequant.o: ../Middlewares/Third_Party/Helix/real/dequant.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/dequant.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/real/dqchan.o: ../Middlewares/Third_Party/Helix/real/dqchan.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/dqchan.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/real/huffman.o: ../Middlewares/Third_Party/Helix/real/huffman.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/huffman.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/real/hufftabs.o: ../Middlewares/Third_Party/Helix/real/hufftabs.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/hufftabs.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/real/imdct.o: ../Middlewares/Third_Party/Helix/real/imdct.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/imdct.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/real/polyphase.o: ../Middlewares/Third_Party/Helix/real/polyphase.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/polyphase.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/real/scalfact.o: ../Middlewares/Third_Party/Helix/real/scalfact.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/scalfact.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/real/stproc.o: ../Middlewares/Third_Party/Helix/real/stproc.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/stproc.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/real/subband.o: ../Middlewares/Third_Party/Helix/real/subband.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/subband.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Third_Party/Helix/real/trigtabs_fixpt.o: ../Middlewares/Third_Party/Helix/real/trigtabs_fixpt.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F746xx -DARM_MATH_CM7 -c -I../Inc -I"C:/workspace/Display/Middlewares/Third_Party/Helix/pub" -I"C:/workspace/Display/Middlewares/Third_Party/Helix/real" -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FatFs/src -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Third_Party/Helix/real/trigtabs_fixpt.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

