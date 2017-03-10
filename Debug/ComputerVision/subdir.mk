################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CU_SRCS += \
../ComputerVision/ComputerVision.cu 

CPP_SRCS += \
../ComputerVision/ComputerVision.cpp 

OBJS += \
./ComputerVision/ComputerVision.o 

CU_DEPS += \
./ComputerVision/ComputerVision.d 

CPP_DEPS += \
./ComputerVision/ComputerVision.d 


# Each subdirectory must supply rules for building sources it contributes
ComputerVision/%.o: ../ComputerVision/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	nvcc -G -g -O0 -gencode arch=compute_30,code=sm_30  -odir "ComputerVision" -M -o "$(@:%.o=%.d)" "$<"
	nvcc -G -g -O0 --compile  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

ComputerVision/%.o: ../ComputerVision/%.cu
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	nvcc -G -g -O0 -gencode arch=compute_30,code=sm_30  -odir "ComputerVision" -M -o "$(@:%.o=%.d)" "$<"
	nvcc -G -g -O0 --compile --relocatable-device-code=false -gencode arch=compute_30,code=compute_30 -gencode arch=compute_30,code=sm_30  -x cu -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


