#	Path with flag, to ever used gcc compiler
CC         = gcc
CPP        = g++

#CFLAGS     = -g -D_REENTRANT -pedantic -Wall -lboost_filesystem -L/usr/lib/x86_64-linux-gnu -L/usr/lib
CFLAGS     = -g -D_REENTRANT -pedantic -Wall -L/usr/lib/x86_64-linux-gnu -L/usr/lib
CPPFLAGS   = $(CFLAGS)

#LINK_FLAGS = -ladm

#	Files to compile?
SRCS       = main.cpp

#	Path to library .o files
#LIB_FILES  = main.o BitDatabase.o BitTestResults.o BitTest.o BitCategory.o
LIB_FILES  = main.o 

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

clean :
	rm -f $(CLEANFILES) $(PROGS)

#
#	What are those dependancies?
#
depend:
	makedepend $(SRCS)

