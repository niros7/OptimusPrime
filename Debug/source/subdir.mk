################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/Configuration.cpp \
../source/Driver.cpp \
../source/Graph.cpp \
../source/Map.cpp \
../source/Particle.cpp \
../source/ParticleManager.cpp \
../source/Robot.cpp \
../source/RobotManager.cpp \
../source/WaypointManager.cpp \
../source/lodepng.cpp \
../source/main.cpp 

OBJS += \
./source/Configuration.o \
./source/Driver.o \
./source/Graph.o \
./source/Map.o \
./source/Particle.o \
./source/ParticleManager.o \
./source/Robot.o \
./source/RobotManager.o \
./source/WaypointManager.o \
./source/lodepng.o \
./source/main.o 

CPP_DEPS += \
./source/Configuration.d \
./source/Driver.d \
./source/Graph.d \
./source/Map.d \
./source/Particle.d \
./source/ParticleManager.d \
./source/Robot.d \
./source/RobotManager.d \
./source/WaypointManager.d \
./source/lodepng.d \
./source/main.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


