TARGET=startup.elf
OBJ := vector_table.o main.o serial.o timer.o task.o console.o sdram.o lcd.o gfx.o fault_handler.o
LDSCRIPT=startup.ld


LIBNAME		= opencm3_stm32f4
OPENCM3_DIR = ../../../../libopencm3
LIBPATHS := ./libopencm3 ../../../../libopencm3 ../../../../../libopencm3
OPENCM3_DIR := $(wildcard $(LIBPATHS:=/locm3.sublime-project))
OPENCM3_DIR := $(firstword $(dir $(OPENCM3_DIR)))
DEFS		+= -I$(OPENCM3_DIR)/include

CROSS=arm-none-eabi-
CFLAGS := -I. $(DEFS) -nostdlib -g -mcpu=cortex-m4 -mthumb -ggdb3 -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Wextra -Wshadow -Wimplicit-function-declaration -Wredundant-decls -Wmissing-prototypes -fno-common -ffunction-sections -fdata-sections  -MD -Wall -Wundef -DSTM32F4
LDFLAGS := -g  -L$(OPENCM3_DIR)lib -l$(LIBNAME)
ASMFLAGS := -g -o


TTY := $(shell tty)
MACH := stm32f4
CDEV := tty,id=any,path=$(TTY)

QEMU_FLAGS := -S -gdb tcp::1234 -nographic -kernel




all: $(TARGET)

%.o: %.s
	$(CROSS)as $(ASMFLAGS) $@ $<

%.o: %.c
	$(CROSS)gcc $(CFLAGS) -c $<

$(TARGET): $(OBJ) $(LDSCRIPT)
	$(CROSS)ld $(LDFLAGS) -T $(LDSCRIPT) $(OBJ) -l$(LIBNAME) -o $@

clean:
	$(RM) *.o *.elf

run: $(TARGET)
	qemu-system-arm -machine $(MACH) $(QEMU_FLAGS) $< -chardev $(CDEV)	

debug: $(TARGET)
	$(CROSS)gdb -x gdbinit
	
	
# st-util
# arm-none-eabi-gdb button.elf
# (gdb) tar extended-remote :4242
# ...
# (gdb) load
# Transfer rate: 1 KB/sec, 560 bytes/write.
# (gdb)
# ...
# (gdb) continue
