################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AF.cpp \
../src/Argument.cpp \
../src/Labelling.cpp \
../src/OrClause.cpp \
../src/Preferred.cpp \
../src/Preferred_pref.cpp \
../src/Preferred_boundcond.cpp \
../src/Preferred_Grounded.cpp \
../src/Preferred_sccsseq.cpp \
../src/SATFormulae.cpp \
../src/SetArguments.cpp \
../src/main.cpp \
../src/misc.cpp 

OBJS += \
./src/AF.o \
./src/Argument.o \
./src/Labelling.o \
./src/OrClause.o \
./src/Preferred.o \
./src/Preferred_pref.o \
./src/Preferred_boundcond.o \
./src/Preferred_Grounded.o \
./src/Preferred_sccsseq.o \
./src/SATFormulae.o \
./src/SetArguments.o \
./src/main.o \
./src/misc.o 

CPP_DEPS += \
./src/AF.d \
./src/Argument.d \
./src/Labelling.d \
./src/OrClause.d \
./src/Preferred.d \
./src/Preferred_pref.d \
./src/Preferred_boundcond.d \
./src/Preferred_Grounded.d \
./src/Preferred_sccsseq.d \
./src/SATFormulae.d \
./src/SetArguments.d \
./src/main.d \
./src/misc.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DNDEBUG -DNLOGPRECO -DNSTATSPRECO -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -I"/Users/mauzuc90/Desktop/Codice_per_studenti_e_osservazioni_varie/SemOpt_0.2alpha2-SCC_100/src" -I"/Users/mauzuc90/Desktop/Codice_per_studenti_e_osservazioni_varie/SemOpt_0.2alpha2-SCC_100/src/precosat" -O0 -g3 -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


