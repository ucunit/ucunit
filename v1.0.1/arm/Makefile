#
# Sample Makefile for uCUnit
#

#----------------------------------------------------
# Toolchain configuration
#----------------------------------------------------
# Compiler
CC:=arm-elf-gcc
CFLAGS:=-mcpu=arm7tdmi -mthumb -g

#Linker
LD:=arm-elf-gcc
LDFLAGS:=-mcpu=arm7tdmi -mthumb -g

# Simulator/Emulator
RUN:=arm-elf-run

# Clean up
RM:=del

#----------------------------------------------------
# Files
#----------------------------------------------------
# Name of target file
TARGET:=Testsuite.elf

# Sourcefiles
SRCS:=           \
./System.c       \
../Testsuite.c   \

# Objectfiles
OBJS:=           \
./System.obj     \
../Testsuite.obj \

#----------------------------------------------------
# Build all
#----------------------------------------------------
all: $(TARGET)

run: $(TARGET)
	echo 'Running $(TARGET)...'
	$(RUN) $(TARGET)
	
#----------------------------------------------------
# Linker stage
#----------------------------------------------------
$(TARGET):	$(OBJS)
	echo 'Linking...'
	$(LD) $(LDFLAGS) -o $(notdir $@) $(notdir $(OBJS))

#----------------------------------------------------
# Compiler
#----------------------------------------------------
%.obj: %.c
	echo 'Compiling...'
	$(CC) $(CFLAGS) -o $(notdir $@) -c $<
	
#----------------------------------------------------
# Clean Project
#----------------------------------------------------
clean:
	echo 'Running clean...'
	$(RM) $(TARGET)
	$(RM) System.obj
	$(RM) Testsuite.obj
	echo 'Clean finished.'
	