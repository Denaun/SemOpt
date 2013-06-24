################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/precosat/precobnr.cc \
../src/precosat/precomain.cc \
../src/precosat/precosat.cc 

OBJS += \
./src/precosat/precobnr.o \
./src/precosat/precomain.o \
./src/precosat/precosat.o 

CC_DEPS += \
./src/precosat/precobnr.d \
./src/precosat/precomain.d \
./src/precosat/precosat.d 


# Each subdirectory must supply rules for building sources it contributes
src/precosat/%.o: ../src/precosat/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DNDEBUG -DNLOGPRECO -DNSTATSPRECO -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -I"/Users/mauzuc90/Desktop/Codice_per_studenti_e_osservazioni_varie/SemOpt_0.2alpha2-SCC_100/src" -I"/Users/mauzuc90/Desktop/Codice_per_studenti_e_osservazioni_varie/SemOpt_0.2alpha2-SCC_100/src/precosat" -O0 -g3 -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


