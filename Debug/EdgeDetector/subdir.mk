################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CU_SRCS += \
../EdgeDetector/stereoDisparity.cu 

OBJS += \
./EdgeDetector/stereoDisparity.o 

CU_DEPS += \
./EdgeDetector/stereoDisparity.d 


# Each subdirectory must supply rules for building sources it contributes
EdgeDetector/%.o: ../EdgeDetector/%.cu
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/Developer/NVIDIA/CUDA-8.0/samples/common/inc -I"/usr/local/cuda-8.0/samples/3_Imaging" -I"/usr/local/cuda-8.0/samples/common/inc" -I"/Users/justin/Cloud/Git/ComputerVision" -G -g -O0 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_30,code=sm_30 -m64 -odir "EdgeDetector" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/Developer/NVIDIA/CUDA-8.0/samples/common/inc -I"/usr/local/cuda-8.0/samples/3_Imaging" -I"/usr/local/cuda-8.0/samples/common/inc" -I"/Users/justin/Cloud/Git/ComputerVision" -G -g -O0 --compile --relocatable-device-code=false -gencode arch=compute_30,code=compute_30 -gencode arch=compute_30,code=sm_30 -m64 -ccbin aarch64-linux-gnu-g++  -x cu -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


