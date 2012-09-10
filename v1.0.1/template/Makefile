#
# Sample Makefile for uCUnit
#

#----------------------------------------------------
# Toolchain configuration
#----------------------------------------------------
# Compiler
CC:=gcc
CFLAGS:=-g

#Linker
LD:=gcc
LDFLAGS:=-g

# Simulator/Emulator
RUN:=cmd /C

# Clean up
RM:=del

#----------------------------------------------------
# Files
#----------------------------------------------------
# Name of target file
TARGET:=Testsuite.exe

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
	