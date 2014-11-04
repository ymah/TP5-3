# $Id: Makefile,v 1.2 2004/10/12 09:06:17 marquet Exp $
##############################################################################

ROOTDIR=.
DIR=.

CC	= gcc
CFLAGS	= -Wall
CFLAGS  += -ansi
CFLAGS  += -pedantic
CFLAGS  += -g
LIBDIR  = $(ROOTDIR)/lib
INCDIR  = -I$(ROOTDIR)/include
LIBS    = -L$(LIBDIR) -lhardware

###------------------------------
### Main targets
###------------------------------------------------------------
BINARIES= vm mkhd
OBJECTS	= $(addsuffix .o,\
	  drive mbr vm)

all: $(BINARIES) $(OBJECTS)


###------------------------------
### Main rules
###------------------------------------------------------------
drive.o: drive.c drive.h mbr.h
mkhd.o:mkhd.c
mbr.o: drive.h mbr.h mbr.c
vm.o: vm.c mbr.h drive.h descripteur.h
descripteur.o:descripteur.c descripteur.h mbr.h drive.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< $(INCDIR)


drive: drive.o mbr.o
mkhd:mkhd.o
mbr:mbr.o drive.o
vm:vm.o drive.o mbr.o descripteur.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)


###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *~ *# *.o $(BINARIES)
realclean: clean
	$(RM) vdiskA.bin vdiskB.bin

