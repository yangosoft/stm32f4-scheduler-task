##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##


# OBJS = sdram.o clock.o console.o lcd-spi.o

# BINARY = lcd-dma
# CSTD = -std=gnu99

# we use sin/cos from the library
LDLIBS += -lm

# LDSCRIPT = ../stm32f429i-discovery.ld


OBJS = sdram.o clock.o lcd.o font-7x12.o gfx.o console.o scheduler.o
#lcd-serial.o lcd-spi.o font-7x12.o gfx.o console.o

OPENCM3_DIR = ../libopencm3

BINARY = multithreading

LDSCRIPT = ./stm32f429i-discovery.ld

include ./Makefile.include

# arm-none-eabi-gdb button.elf
# (gdb) tar extended-remote :4242
# ...
# (gdb) load
# Loading section .text, size 0x458 lma 0x8000000
# Loading section .data, size 0x8 lma 0x8000458
# Start address 0x80001c1, load size 1120
# Transfer rate: 1 KB/sec, 560 bytes/write.
# (gdb)
# ...
# (gdb) continue
