#	Path with flag, to ever used gcc compiler
CC         = gcc
CPP        = g++

INC_DIRS   = -I. -Iunity
LIBFLAGS   = -L/usr/lib/x86_64-linux-gnu -L/usr/lib -ldl
CFLAGS     = -g -D_REENTRANT -pedantic -Wall -pthread $(LIBFLAGS)
CPPFLAGS   = $(CFLAGS)

#LINK_FLAGS = -ladm

#	Files to compile?
SRCS       = main.c

#	Path to library .o files
#LIB_FILES  = main.o BitDatabase.o BitTestResults.o BitTest.o BitCategory.o
LIB_FILES  = main.o listdir.o work_queue.o

TARGET1 = test1.out
UNIT_TEST_FILE = TestProductionCode.c
UNIT_TEST_AUTOGEN_RUNNER = TestProductionCode_Runner.c
SRC_FILES1=unity/unity.c $(UNIT_TEST_AUTOGEN_RUNNER) $(UNIT_TEST_FILE) listdir.c work_queue.c linked_list.c

CLEANFILES = core core*.* *.core *.o temp.* *.out typescript* \
		*.[234]c *.[234]h *.bsdi *.sparc *.uw

#	OUTPUTTED Executables
PROGS      = ssfi

all :	$(LIB_FILES) $(PROGS) $(TARGET1) 
	./$(TARGET1)

exe :	$(LIB_FILES) $(PROGS)
	./$(PROGS) -t 3 .

.c.o :
	$(CC) $(CFLAGS) -c $<
.cpp.o :
	$(CPP) $(CPPFLAGS) -c $<

ssfi : $(LIB_FILES)
	$(CC) $(CPPFLAGS) $(LINK_FLAGS) -o ssfi $(LIB_FILES)

test: $(SRC_FILES1)
	$(CC) -g $(INC_DIRS) -DTEST $(SRC_FILES1) -o $(TARGET1)
	./$(TARGET1)

# Rule to generate runner file automatically
$(UNIT_TEST_AUTOGEN_RUNNER): $(UNIT_TEST_FILE)
	ruby unity/auto/generate_test_runner.rb $(UNIT_TEST_FILE) $(UNIT_TEST_AUTOGEN_RUNNER)

clean :
	rm -f $(CLEANFILES) $(PROGS)

#
#	What are those dependancies?
#
depend:
	makedepend $(SRCS)

