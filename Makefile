CC = arm-none-eabi-gcc

# The location of the C compiler
# ARMGCC_ROOT is used by some makefiles that need to know where the compiler
# is installed.
ARMGCC_ROOT := ${shell dirname ${shell readlink ${shell which ${CC}}}}/..

ROOT ?= $(abspath ..)

OBJECTS = main.o system.o startup.o

NAME = lab

CFLAGS = -I.. -I../inc \
    -D__MSP432P401R__ \
    -DDeviceFamily_MSP432P401x \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -std=c99 \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -ffunction-sections \
    -fdata-sections \
    -g \
    -gstrict-dwarf \
    -Wall \
    -I$(ARMGCC_ROOT)/arm-none-eabi/include/newlib-nano \
    -I$(ARMGCC_ROOT)/arm-none-eabi/include

LFLAGS = -Wl,-T,../config.lds \
    -Wl,-Map,$(NAME).map \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -static \
    -Wl,--gc-sections \
	-lgcc \
    -lc \
    -lm \
    -lnosys \
    --specs=nano.specs

all: $(NAME).out

main.o: main.c
	@$(CC) $(CFLAGS) $< -c -o $@

system.o: ../system.c
	@$(CC) $(CFLAGS) $< -c -o $@

startup.o: ../startup.c
	@$(CC) $(CFLAGS) $< -c -o $@

$(NAME).out: $(OBJECTS)
	@$(CC) $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean: # Redirecting both the stderr and stdout to /dev/null 
	@rm -f $(OBJECTS) > /dev/null 2>&1
	@rm -f $(NAME).out > /dev/null 2>&1
	@rm -f $(NAME).map > /dev/null 2>&1
