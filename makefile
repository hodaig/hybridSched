# All Target
all: bin/test

# Add inputs and outputs from these tool invocations to the build variables 

OBJS += \
./bin/HSAutomata.o \
./bin/HSGoalAutomata.o \
./bin/HSMode.o \
./bin/HSTransition.o \
./bin/HybridSched.o 

# Each subdirectory must supply rules for building sources it contributes
bin/%.o: ./%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

OBJS += \
./bin/tests/Main.o \
./bin/tests/TestAutoUtil.o \
./bin/tests/Utils.o 

OBJS += \
./bin/conditions/HSCondition.o \
./bin/conditions/HSConditionAND.o \
./bin/conditions/HSConditionFalse.o \
./bin/conditions/HSConditionFunc.o \
./bin/conditions/HSConditionNot.o \
./bin/conditions/HSConditionSmallEq.o \
./bin/conditions/HSConditionTrue.o 

OBJS += \
./pugixml-1.7/src/pugixml.o 

# Tool invocations
bin/test: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o "bin/test" $(OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '


# Other Targets
clean:
	rm bin/test \
	bin/*.o \
	bin/*.d \
	bin/tests/*.o \
	bin/tests/*.d \
	bin/conditions/*.o \
	bin/conditions/*.d

.PHONY: all clean