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
RUN:=

# Clean up
RM:=rm

# Silencer
SILENT:=@

#----------------------------------------------------
# Files
#----------------------------------------------------
# Name of target files
TARGET:=./ListSampleSpec.exe

# Sourcefiles
SRCS:=           \
../System.c          \
../../ListSample.c  \
../../ListSampleSpec.c \

# Objectfiles
OBJS:= \
../System.obj         \
../../ListSample.obj  \
../../ListSampleSpec.obj \

#----------------------------------------------------
# Build all
#----------------------------------------------------
all: $(TARGET)

run: $(TARGET)
	$(SILENT)echo 'Running $(TARGET)...'
	$(RUN) $(TARGET)
	
#----------------------------------------------------
# Linker stage
#----------------------------------------------------
$(TARGET):	$(OBJS)
	$(SILENT)echo 'Linking...'
	$(LD) $(LDFLAGS) -o $(notdir $@) $(notdir $(OBJS))

#----------------------------------------------------
# Compiler
#----------------------------------------------------
%.obj: %.c
	$(SILENT)echo 'Compiling...'
	$(CC) $(CFLAGS) -o $(notdir $@) -c $<
	
#----------------------------------------------------
# Clean Project
#----------------------------------------------------
clean:
	$(SILENT)echo 'Running clean...'
	$(RM) $(TARGET)
	$(RM) System.obj
	$(RM) ListSample.obj
	$(RM) ListSampleSpec.obj
	$(SILENT)echo 'Clean finished.'
	