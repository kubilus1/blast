GENDEV?=/opt/toolchains/gen/
MAKE?=make
GENBIN=$(GENDEV)/bin
#GENGCC_BIN=$(GENDEV)/m68k-elf/bin
GENGCC_BIN=$(GENDEV)/bin

CC = $(GENGCC_BIN)/m68k-elf-gcc 
AR = $(GENGCC_BIN)/m68k-elf-ar 
RANLIB = $(GENGCC_BIN)/m68k-elf-ranlib 
OBJC = $(GENGCC_BIN)/m68k-elf-objcopy 
RM = rm -f 
ASMZ80 = $(GENBIN)/sjasm 
BINTOS = $(GENBIN)/bintos 

OPTION= -Dnologo_ --save-temps -fverbose-asm -g 

CS=$(wildcard libs/*.c)
SS=$(wildcard libs/*.s)
S80S=$(wildcard libs/*.s80)
RESOURCES=$(CS:.c=.o)
RESOURCES+=$(SS:.s=.o)
RESOURCES+=$(S80S:.s80=.o)

blast.a_OBJS=$(RESOURCES)

INCS = -Iincs -I$(GENDEV)/m68k-elf/include -I$(GENDEV)/sgdk/inc -I$(GENDEV)/sgdk/res
FLAGS = $(OPTION) -m68000 -Wall -O1 -c -fomit-frame-pointer $(INCS) 
FLAGSZ80 = -isrc -iincs -I$(GENDEV)/m68k-elf/include

all: libblast.a

%.a: $(blast.a_OBJS)
	$(RM) $@ 
	$(AR) cru $@ $(blast.a_OBJS) 
	$(RANLIB) $@ 

%.o80: %.s80 
	$(ASMZ80) $(FLAGSZ80) $< $@ out.lst 

%.s: %.o80 
	$(BINTOS) $< 

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(FLAGS) -c $< -o $@

install: libblast.a
	mkdir -p $(GENDEV)/include/blast
	cp libblast.a $(GENDEV)/m68k-elf/lib
	cp incs/*.h $(GENDEV)/include/blast

clean:
	echo "Clean"
	-rm libs/*.o
	-rm libs/*.a
	-rm libblast.a
