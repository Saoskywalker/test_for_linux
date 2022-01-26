include mkenv.mk

#define
DEFINES	+= #-D__ARM32_ARCH__=5 -D__ARM926EJS__

#include path
INCDIRS	+= #-Imachine/include -Iarch/arm32/include -Imachine/include/f1c100s -Idriver/include -Ilib

#library path
LIBDIRS	+=

#library
LIBS += -lgcc -lmingw32 -lSDL2main -lSDL2

#c source path
SRCDIRS_C += 
SRC_C += $(wildcard $(foreach n, $(SRCDIRS_C), $(n)/*.c))

#c source files
SRC_C += main.c ts_calibrate_common.c cJSON.c MTF_file.c dwDriver.c dwDriveD2.c DW_FileDecodeLib.c test_sdl.c

#c++ source path
SRCDIRS_CXX += 
SRC_CXX += $(wildcard $(foreach n, $(SRCDIRS_CXX), $(n)/*.cpp))

#c++ source files
SRC_CXX += 

#asm source files
SRC_ASM += 


OBJ = $(addprefix $(BUILD)/, $(SRC_C:.c=.o)) $(addprefix $(BUILD)/, $(SRC_ASM:.S=.o)) $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))

-include $(addprefix $(BUILD)/, $(SRC_C:.c=.d)) $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.d))

define compile_c
$(ECHO) "CC $<"
$(Q)$(CC) $(INCDIRS) $(MCFLAGS) $(CFLAGS) -c -MD -o $@ $<
@# The following fixes the dependency file.
@# See http://make.paulandlesley.org/autodep.html for details.
@# Regex adjusted from the above to play better with Windows paths, etc.
@$(CP) $(@:.o=.d) $(@:.o=.P); \
  $(SED) -e 's/#.*//' -e 's/^.*:  *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P);
endef

# define compile_c
# $(ECHO) "CC $<"
# $(CC) $(INCDIRS) $(MCFLAGS) $(CFLAGS) -c -MD -o $@ $<
# @# The following fixes the dependency file.
# @# See http://make.paulandlesley.org/autodep.html for details.
# @# Regex adjusted from the above to play better with Windows paths, etc.
# @$(CP) $(@:.o=.d) $(@:.o=.P); \
#   $(SED) -e 's/#.*//' -e 's/^.*:  *//' -e 's/ *\\$$//' \
#       -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); \
#   $(RM) -f $(@:.o=.d)
# endef

define compile_cxx
$(ECHO) "CXX $<"
$(Q)$(CXX) $(INCDIRS) $(MCFLAGS) $(CXXFLAGS) -c -MD -o $@ $<
@# The following fixes the dependency file.
@# See http://make.paulandlesley.org/autodep.html for details.
@# Regex adjusted from the above to play better with Windows paths, etc.
@$(CP) $(@:.o=.d) $(@:.o=.P); \
  $(SED) -e 's/#.*//' -e 's/^.*:  *//' -e 's/ *\\$$//' \
      -e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P);
endef

$(BUILD)/%.o: %.c
	$(call compile_c)

$(BUILD)/%.o: %.cpp
	$(call compile_cxx)

$(BUILD)/%.o: %.S
	$(ECHO) "AS $<"
	$(Q)$(AS) $(MCFLAGS) $(ASFLAGS) -c -o $@ $<
	
$(BUILD)/%.o: %.s
	$(ECHO) "AS $<"
	$(Q)$(AS) $(MCFLAGS) $(CFLAGS) -o $@ $<

OBJ_DIRS = $(sort $(dir $(OBJ)))
$(OBJ): | $(OBJ_DIRS)
$(OBJ_DIRS):
	$(MKDIR) -p $@

$(BUILD)/$(addprefix $(PROG), $(EXTENSION)): $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -Wl,--cref,-Map=$@.map -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/$(addprefix $(PROG), .bin): $(BUILD)/$(addprefix $(PROG), $(EXTENSION))
	$(Q)$(OBJCOPY) -v -O binary $^ $@
	$(ECHO) Make header information for brom booting
	@$(MKSUNXI) $@
