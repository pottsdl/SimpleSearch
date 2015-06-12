CC         = gcc
CPP        = g++

INC_DIRS   = -I. -Iunity
LIBFLAGS   = -L/usr/lib -L /usr/local/lib -L/usr/lib/x86_64-linux-gnu
CFLAGS     = -g -D_REENTRANT -pedantic -Wall -pthread $(LIBFLAGS)
CPPFLAGS   = $(CFLAGS)

LINK_FLAGS = -lstdc++

#	Files to compile
SRCS       = main.c

#	Path to library .o files
LIB_FILES  = main.o listdir.o work_queue.o list_test.o

TEST_TARGET = test1.out
UNIT_TEST_FILE = TestProductionCode.c
UNIT_TEST_AUTOGEN_RUNNER = TestProductionCode_Runner.c
UNITTEST_SRC_FILES=unity/unity.c $(UNIT_TEST_AUTOGEN_RUNNER) $(UNIT_TEST_FILE) work_queue.cpp

CLEANFILES = core core*.* *.core *.o temp.* *.out typescript* \
		*.[234]c *.[234]h *.bsdi *.sparc *.uw

#	OUTPUTTED Executables
PROGS      = ssfi

all :	$(LIB_FILES) $(PROGS) $(TEST_TARGET) 
	./$(TEST_TARGET)

exe :	$(LIB_FILES) $(PROGS)
	./$(PROGS) -t 3 .

.c.o :
	$(CC) $(CFLAGS) -c $<
.cpp.o :
	$(CPP) $(CPPFLAGS) -c $<

ssfi : $(LIB_FILES)
	$(CPP) $(CPPFLAGS) $(LINK_FLAGS) -o ssfi $(LIB_FILES)

test: $(UNITTEST_SRC_FILES)
	$(CPP) -g -pthread $(INC_DIRS) -DTEST $(UNITTEST_SRC_FILES) -o $(TEST_TARGET)
	./$(TEST_TARGET)

# Rule to generate runner file automatically
$(UNIT_TEST_AUTOGEN_RUNNER): $(UNIT_TEST_FILE)
	ruby unity/auto/generate_test_runner.rb $(UNIT_TEST_FILE) $(UNIT_TEST_AUTOGEN_RUNNER)

clean :
	rm -f $(CLEANFILES) $(PROGS)
