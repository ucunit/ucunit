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
RM:=rm -f

#----------------------------------------------------
# Files
#----------------------------------------------------
# Name of target file
TARGET:=Testsuite.exe

# Sourcefiles
SRCS:=           \
./System.c       \
./Testsuite.c   \
./uCUnit-xml.c	\

# Objectfiles
OBJS:=           \
./template/System.o     \
./Testsuite.o \
./uCUnit-xml.o	\


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
	$(CC) -o $(TARGET) $^ $(LDFLAGS)

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
	$(RM) **/*.o core
	$(RM) *.o core
	echo 'Clean finished.'
	