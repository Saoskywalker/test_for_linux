.PHONY:clean
.PHONY:write
.PHONY:mktool
.PHONY:make_release
.PHONY:make_debug
.PHONY:make_embed
.PHONY:dump
.PHONY:run

BUILD ?= build
RM = rm
ECHO = @echo
CP = cp
MKDIR = mkdir
SED = sed
PYTHON = python

CROSS_COMPILE = arm-eabi-
CC	          = $(CROSS_COMPILE)gcc
AS	          = $(CROSS_COMPILE)gcc -x assembler-with-cpp
LD	          = $(CROSS_COMPILE)ld
OBJCOPY	      = $(CROSS_COMPILE)objcopy
OBJDUMP       = $(CROSS_COMPILE)objdump
SIZE 		  = $(CROSS_COMPILE)size

#define
DEFINES		+= -D__ARM32_ARCH__=5 -D__ARM926EJS__

ASFLAGS		:= -g -ggdb -Wall -O3 -ffreestanding -std=gnu99 $(DEFINES)
CFLAGS		:= -g -ggdb -Wall -O3 -ffreestanding -std=gnu99 $(DEFINES)
CXXFLAGS	:= -g -ggdb -Wall -O3 -ffreestanding -std=gnu99 $(DEFINES)
LDFLAGS		:= -T f1c100s.ld -nostdlib
MCFLAGS		:= -march=armv5te -mtune=arm926ej-s -mfloat-abi=soft -marm -mno-thumb-interwork

#include path
INCDIRS		:= -Imachine/include -Iarch/arm32/include -Imachine/include/f1c100s -Idriver/include -Ilib

#library path
LIBDIRS		:=

#library
LIBS 		:= -lgcc

#source path
SRCDIRS		:=

#c source files
SRC_C = 

#asm source files
SRC_ASM = 

OBJ = $(addprefix $(BUILD)/, $(SRC_C:.c=.o)) $(addprefix $(BUILD)/, $(SRC_ASM:.S=.o))

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(OBJCOPY) -v -O binary $^ $@
	@echo Make header information for brom booting
	@$(MKSUNXI) $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(CC) $(LDFLAGS) -Wl,--cref,-Map=$@.map -o $@ $^ $(LIBS)
	$(SIZE) $@

$(BUILD)/%.o: %.S
	$(ECHO) "AS $<"
	$(AS) $(MCFLAGS) $(ASFLAGS) -c -o $@ $<
	
$(BUILD)/%.o: %.s
	$(ECHO) "AS $<"
	$(AS) $(MCFLAGS) $(CFLAGS) -o $@ $<

define compile_c
$(ECHO) "CC $<"
$(CC) $(INCDIRS) $(MCFLAGS) $(CFLAGS) -c -MD -o $@ $<
@# The following fixes the dependency file.
@# See http://make.paulandlesley.org/autodep.html for details.
@# Regex adjusted from the above to play better with Windows paths, etc.
@$(CP) $(@:.o=.d) $(@:.o=.P); \
  $(SED) -e 's/#.*//' -e 's/^.*:  *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); \
  $(RM) -f $(@:.o=.d)
endef

$(BUILD)/%.o: %.c
	$(call compile_c)

OBJ_DIRS = $(sort $(dir $(OBJ)))
$(OBJ): | $(OBJ_DIRS)
$(OBJ_DIRS):
	$(MKDIR) -p $@

CC:=6
DD = fdsf
CC = 654
#rwer

make_debug: 
#fdgs
	@ echo "*************************************************"
	@ echo "*************************************************"
	@ echo "*************************************************"
	@ echo             make_debug     start
	@ echo compiling iii $(DD) $(CC) $@
	@ echo "*************************************************"
	gcc -g -std=gnu99 -Wall -o OUTPUT/main_debug main.c ts_calibrate_common.c cJSON.c MTF_file.c dwDriver.c -lmingw32 -lSDL2main -lSDL2
	@ echo "<><><><><><>><><>><><><><><><><><><>><><><><><><>"
	@ echo            make_debug  compile ok
	@ echo "<><><><><><>><><>><><><><><><><><><><><><><<><><>"

make_release: 
#fdgs
	@ echo "*************************************************"
	@ echo "*************************************************"
	@ echo "*************************************************"
	@ echo             make_release     start
	@ echo compiling iii $(DD) $(CC) $@
	@ echo "*************************************************"
	gcc -std=gnu99 -O3 -Wall -o OUTPUT/main_release main.c ts_calibrate_common.c cJSON.c MTF_file.c dwDriver.c -lmingw32 -lSDL2main -lSDL2
	@ echo "<><><><><><>><><>><><><><><><><><><>><><><><><><>"
	@ echo            make_release  compile ok
	@ echo "<><><><><><>><><>><><><><><><><><><><><><><<><><>"

make_embed: $(BUILD)/firmware.bin
	@ echo "<><><><><><>><><>><><><><><><><><><>><><><><><><>"
	@ echo            $@  compile ok
	@ echo "<><><><><><>><><>><><><><><><><><><><><><><<><><>"

run:
	@ echo run ./OUTPUT/test
	./OUTPUT/test

clean:
	find . -name "*.o"  | xargs rm -f  
	find . -name "*.bin"  | xargs rm -f
	find . -name "*.elf"  | xargs rm -f
	find . -name "*.P"  | xargs rm -f

dump:
	$(OBJDUMP) -S myboot.o | less

write:
	sudo sunxi-fel -p spiflash-write 0 $(BUILD)/firmware.bin

mktool:
	cd tools/mksunxiboot && make
	cd tools/mksunxi && make

MKSUNXI		:= tools/mksunxi/mksunxi

mkboot:
	@echo Make header information for brom booting
	@$(MKSUNXI) $(BUILD)/firmware.bin
