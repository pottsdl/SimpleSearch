#	Path with flag, to ever used gcc compiler
CC         = gcc
CPP        = g++

INC_DIRS   = -I. -Iunity
LIBFLAGS   = -L/usr/lib/x86_64-linux-gnu -L/usr/lib -ldl
CFLAGS     = -g -D_REENTRANT -pedantic -Wall $(LIBFLAGS)
CPPFLAGS   = $(CFLAGS)

#LINK_FLAGS = -ladm

#	Files to compile?
SRCS       = main.c

#	Path to library .o files
#LIB_FILES  = main.o BitDatabase.o BitTestResults.o BitTest.o BitCategory.o
LIB_FILES  = main.o listdir.o

CLEANFILES = core core*.* *.core *.o temp.* *.out typescript* \
		*.[234]c *.[234]h *.bsdi *.sparc *.uw

#	OUTPUTTED Executables
PROGS      = ssfi

all :	$(LIB_FILES) $(PROGS)

.c.o :
	$(CC) $(CFLAGS) -c $<
.cpp.o :
	$(CPP) $(CPPFLAGS) -c $<

ssfi : $(LIB_FILES)
	$(CPP) $(CPPFLAGS) $(LINK_FLAGS) -o ssfi $(LIB_FILES)

TARGET1 = test1.out
SRC_FILES1=unity/unity.c listdir.c TestProductionCode_Runner.c TestProductionCode.c
test: 
	$(CC) $(INC_DIRS) -DTEST $(SRC_FILES1) -o $(TARGET1)
	./$(TARGET1)


clean :
	rm -f $(CLEANFILES) $(PROGS)

#
#	What are those dependancies?
#
depend:
	makedepend $(SRCS)

