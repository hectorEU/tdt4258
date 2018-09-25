# Example Makefile
#
# Exercise 1, TDT4258

LD=arm-none-eabi-gcc
AS=arm-none-eabi-as
OBJCOPY=arm-none-eabi-objcopy

LDFLAGS=-nostdlib
ASFLAGS=-mcpu=cortex-m3 -mthumb -g -am
CFLAGS=

LINKERSCRIPT=efm32gg.ld

.PHONY : improved baseline
# This flag will solve the macro in ex1.S
improved : CFLAGS=-DIMPROVED
improved : ex1.bin
baseline : ex1.bin

# Get raw binary
ex1.bin : ex1.elf
	${OBJCOPY} -j .text -O binary $< $@

# Linker stage
ex1.elf : ex1.o
	${LD} -T ${LINKERSCRIPT} $^ -o $@ ${LDFLAGS}

# Use preprocessor and compile
ex1.o : ex1.S
	${LD} -E ${CFLAGS} $< -o ex1_post.s
	${AS} ${ASFLAGS} ex1_post.s -o $@

.PHONY : upload
upload :
	-eACommander.sh -r --address 0x00000000 -f "ex1.bin" -r

.PHONY : clean
clean :
	-rm -rf *.o *.elf *.bin *.hex
