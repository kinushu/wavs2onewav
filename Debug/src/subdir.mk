################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CWavFileReader.cpp \
../src/CWaveFileWriter.cpp \
../src/wavmerge.cpp 

OBJS += \
./src/CWavFileReader.o \
./src/CWaveFileWriter.o \
./src/wavmerge.o 

CPP_DEPS += \
./src/CWavFileReader.d \
./src/CWaveFileWriter.d \
./src/wavmerge.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


