###############################################################################
# Makefile for the project uWebSrv
###############################################################################

## General Flags
PROJECT = uWebSrv
MCU = atmega32
TARGET = uWebSrv.elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=uWebSrv.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = Enc28j60.o Spi.o uip.o uip_arp.o uip_TcpAppHub.o uip_UdpAppHub.o Main.o Gpio.o led_matrix.o lms.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) uWebSrv.hex uWebSrv.eep uWebSrv.lss size

## Compile
Enc28j60.o: ./Hardware/Enc28j60.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Spi.o: ./Hardware/Spi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uip.o: ./Net/uip/uip.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uip_arp.o: ./Net/uip/uip_arp.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uip_TcpAppHub.o: ./Net/uip/uip_TcpAppHub.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uip_UdpAppHub.o: ./Net/uip/uip_UdpAppHub.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Main.o: ./Main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Gpio.o: ./Hardware/Gpio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

led_matrix.o: led_matrix.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lms.o: ./Net/TcpApps/lms.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
#	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) uWebSrv.elf dep/* uWebSrv.hex uWebSrv.eep uWebSrv.lss uWebSrv.map


AVRDUDE_PROGRAMMER = usbasp


AVRDUDE_PORT = /dev/ttyS0          # programmer connected to serial device
#AVRDUDE_PORT = lpt1    # prgrammer connected to parallel port

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
	#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE = avrdude

program: $(TARGET).hex $(TARGET).eep
	        $(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

