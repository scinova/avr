NAME=blink
DEVICE=atmega2560
DEVICE=atmega328p
#DEVICE=attiny85
F_CPU=16000000L

CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
STRIP=avr-strip
LD=avr-ld

CFLAGS = -fdata-sections -ffunction-sections
CFLAGS += -g -Os -DF_CPU=$(F_CPU) -mmcu=$(DEVICE)
CFLAGS += -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function
CFLAGS += -D I2C_TXN_BUFFER_SIZE=10 -D I2C_TX_BUFFER_SIZE=20
LDFLAGS = -Wl,--gc-sections -lc

BUILDDIR=build
C_FILES = $(wildcard *.c)
CPP_FILES = $(wildcard *.cpp)
ASM_FILES = $(wildcard *.S)
C_OBJS = $(addprefix $(BUILDDIR)/,$(patsubst %.c,%.c.o,$(C_FILES)))
CPP_OBJS = $(addprefix $(BUILDDIR)/,$(patsubst %.cpp,%.cpp.o,$(CPP_FILES)))
ASM_OBJS = $(addprefix $(BUILDDIR)/,$(patsubst %.S,%.S.o,$(ASM_FILES)))
ALL_OBJS = $(ASM_OBJS) $(C_OBJS) $(CPP_OBJS)

all: dir $(BUILDDIR)/$(NAME).hex

dir:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.c.o: %.c
	$(CC) -std=c99 $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.cpp.o: %.cpp
	$(CC) -std=c++11 $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.S.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/$(NAME).elf: $(ALL_OBJS)
	$(CC) -g $(CFLAGS) $(LDFLAGS) -o $(BUILDDIR)/$(NAME).elf $(ALL_OBJS)

$(BUILDDIR)/$(NAME).hex: $(BUILDDIR)/$(NAME).elf
#	$(STRIP) -s --strip-unneeded --only-keep-debug $(BUILDDIR)/$(NAME).elf
	avr-size --mcu=$(DEVICE) -C --format=avr $(BUILDDIR)/$(NAME).elf
	$(OBJDUMP) -h -D -S $(BUILDDIR)/$(NAME).elf > $(BUILDDIR)/$(NAME).lst
	$(OBJCOPY) -j .text -j .data -O ihex $(BUILDDIR)/$(NAME).elf $(BUILDDIR)/$(NAME).hex
#	cp $(BUILDDIR)/$(NAME).hex /sdcard/ArduinoDroid/precompiled

clean:
	@if [ -d $(BUILDDIR) ]; then rm -r $(BUILDDIR); fi
