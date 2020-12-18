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
TARGET:=Testsuite

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

# Dependencies

DEPS:=			\

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
	$(CC) -o $(notdir $@) -c $< $(CFLAGS)
	
#----------------------------------------------------
# Clean Project
#----------------------------------------------------
clean:
	echo 'Running clean...'
	$(RM) ./$(TARGET)
	$(RM) ./**/*.o core
	$(RM) ./*.o core
	$(RM) ../*.xml core
	echo 'Clean finished.'
	