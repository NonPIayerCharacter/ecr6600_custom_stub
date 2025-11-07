# build options
EX9 ?= NO

## debug: YES, NO
DEBU ?= NO

# tool-chain
CROSS_COMPILE ?= nds32le-elf-
CROSSDEV = ../OpenBK7231T_App/sdk/OpenECR6600/tool/nds32le-elf-mculib-v3s/bin/$(CROSS_COMPILE)

CC = $(CROSSDEV)gcc
OBJCOPY = $(CROSSDEV)objcopy
OBJDUMP = $(CROSSDEV)objdump
NM = $(CROSSDEV)nm

SRC_PATH = $(shell pwd)
LD_PATH  = $(shell pwd)

VPATH	:=  $(SRC_PATH)
INCLUDE	:= -I$(SRC_PATH)/include -I$(SRC_PATH)/lib/bootrom
CSRCS	:= 
ASRCS	:=
SRCS	:=


###########################################
#
#			host machine os
#
###########################################
HOSTOS = ${shell uname -o 2>/dev/null || echo "Other"}
ifeq ($(HOSTOS),Cygwin)
HOSTEXEEXT ?= .exe
endif


#stub
CSRCS += main.c uboot_cmd.c
CSRCS += uboot_init.c hal_spiflash.c uboot_version.c efuse.c trng.c
CSRCS += env.c

#ASRCS
VPATH += bsp
ASRCS += crt0.S


# output-path & obj-path
OUT_PATH = $(SRC_PATH)/out
OBJ_PATH = $(OUT_PATH)/obj


#objs
OBJS := $(patsubst %.S,$(OBJ_PATH)/%.S.o,$(ASRCS))
OBJS += $(patsubst %.c,$(OBJ_PATH)/%.c.o,$(CSRCS))
OBJS += $(patsubst %.cpp,$(OBJ_PATH)/%.o,$(SRCS))

DEPS := $(patsubst %.S,$(OBJ_PATH)/%.S.d,$(ASRCS))
DEPS += $(patsubst %.c,$(OBJ_PATH)/%.c.d,$(CSRCS))
DEPS += $(patsubst %.cpp,$(OBJ_PATH)/%.d,$(SRCS))

#$(info OBJS is $(OBJS))
#link stript
LDSCRIPT = stub.ld

#flag
AFLAGS =
CFLAGS = -g0 -Os -c -Wall -Wno-strict-aliasing -Wl,--mno-ex9
LDFLAGS = -Os -nostartfiles -nostdlib -static -T $(LDSCRIPT)
OBJCOPYFLAGS = -O binary -R .note -R .comment -S
NMFLAGS = -B -n



# uboot version
UBOOT_V ?= 1.0.0
UBOOT_V_STD := ECR6600F_customstub_V
CFLAGS += -DUBOOT_VERSION='"$(UBOOT_V_STD)$(UBOOT_V)"'
CFLAGS += -DSTUB
BIN = $(UBOOT_V_STD)$(UBOOT_V)

ifdef DBGLOG_LEVEL
CFLAGS += -DDBGLOG_LEVEL=$(DBGLOG_LEVEL)
endif



#STATIC_LINK_FLAGS += -Wl,--no-whole-archive -l:libc_rom.a -l:libgcc_rom.a -l:libm_rom.a
LIB_PATH := $(SRC_PATH)/lib/mcmodel-large
INCLUDE_LIB := -L $(LIB_PATH)  $(STATIC_LINK_FLAGS)

# check gcc version for extra CFLAGS & LDFLAGS
VERSION := $(shell $(CC) --version | grep ^$(CC) | sed 's/^.* //g')
GCC_VERSION := $(shell echo $(VERSION)| sed -e 's/\.\([0-9][0-9]\)/\1/g' -e 's/\.\([0-9]\)/0\1/g' -e 's/^[0-9]\{3,4\}$$/&00/' )

# EX9 feature check, 
ifeq ($(shell echo | $(CC) -E -dM - | grep '\<__NDS32_EXT_EX9__\>' > /dev/null && echo "EX9"), EX9)
ifeq (NO,$(EX9))
CEXTLDFLAGS += -Wl,--mno-ex9
CFLAGS += -DCONFIG_NO_NDS32_EXT_EX9
endif
else
CFLAGS += -DCONFIG_NO_NDS32_EXT_EX9
endif


# debug support
ifeq (YES,$(DEBU))
CFLAGS := $(subst -g3 -Os,-g -O0,$(CFLAGS))
LDFLAGS := $(subst -Os,-O0,$(LDFLAGS))
endif

all: clean fullmask

fullmask: $(OUT_PATH) $(OBJ_PATH) $(OBJS)
	@echo "/*------------------------------------------------- "
	@echo "------------build fullmask stub------------------ "
	@echo "---------------------------------------------------*/ "
	@$(CC) $(LDFLAGS) $(CEXTLDFLAGS) $(OBJS) $(INCLUDE_LIB) -Wl,-Map=$(OUT_PATH)/$(BIN).map -o $(OUT_PATH)/$(BIN).elf
#	@$(OBJCOPY) $(OBJCOPYFLAGS) $(OUT_PATH)/$(BIN).elf $(OUT_PATH)/$(BIN)_noheader.bin
	@$(OBJDUMP) -S -C --demangle $(OUT_PATH)/$(BIN).elf > $(OUT_PATH)/$(BIN).list
	@echo "trstool"
	@./tools/trstool$(HOSTEXEEXT) elf2image out/$(BIN).elf --keyfile ./tools/key.pem --crc_enable True --image_type STUB --release_version no out/$(BIN).bin
#	@python $(SRC_PATH)/tools/mkstub.py mkstub -i $(OUT_PATH)/$(BIN).bin -o $(OUT_PATH)/base64stub
#	@$(SRC_PATH)/tools/bin2hex_new.pl  32 $(OUT_PATH)/$(BIN).bin $(OUT_PATH)/$(BIN)_32_bin

$(OUT_PATH):
	@echo "mkdir -p $@"
	@mkdir -p $@

$(OBJ_PATH):
	@echo "mkdir -p $@"
	@mkdir -p $@

$(OBJ_PATH)/%.o:%.cpp $(OBJ_PATH)
	@echo "compile $<"
	@$(CC) -E -MMD $(CFLAGS) $(CPPFLAGS) -c $< -o $@.i
	@$(CC) -MMD $(CFLAGS) $(INCLUDE) $(CPPFLAGS) -c $< -o $@

$(OBJ_PATH)/%.c.o:%.c $(OBJ_PATH)
	@echo "compile $<"
	@$(CC) -MMD $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJ_PATH)/%.S.o:%.S $(OBJ_PATH) 
	@echo "compile $<"
	@$(CC) -MMD $(CFLAGS) $(INCLUDE) $(ASFLAGS) -c $< -o $@

.PHONY:  fullmaskclean

fullmaskclean:
	@rm -rf $(OUT_PATH)
	@echo "clean fullmask OK"	
	
clean:  fullmaskclean
	@echo "clean all OK"
