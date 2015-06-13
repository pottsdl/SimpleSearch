CC         = gcc
CPP        = g++

INC_DIRS   = -I. -Iunity
LIBFLAGS   = -L/usr/lib -L /usr/local/lib -L/usr/lib/x86_64-linux-gnu
CFLAGS     = -g -D_REENTRANT -pedantic -Wall -pthread $(LIBFLAGS)
CPPFLAGS   = $(CFLAGS) -ansi

LINK_FLAGS = -lstdc++
DOXYGEN_BIN = $(shell which doxygen)
GDB_BIN     = $(shell which gdb)

#	Files to compile
SRCS       = main.cpp

#	Path to library .o files
LIB_FILES  = main.o listdir.o work_queue.o buffer_processing.o word_dict.o

TEST_TARGET = test1.out
UNIT_TEST_FILE = TestProductionCode.c
UNIT_TEST_AUTOGEN_RUNNER = TestProductionCode_Runner.c
UNITTEST_SRC_FILES=unity/unity.c $(UNIT_TEST_AUTOGEN_RUNNER) $(UNIT_TEST_FILE) work_queue.cpp buffer_processing.cpp word_dict.cpp

CLEANFILES = core core*.* *.core *.o temp.* *.out typescript* \
		*.[234]c *.[234]h *.bsdi *.sparc *.uw

#	OUTPUTTED Executables
PROGS      = ssfi

all :	$(LIB_FILES) $(PROGS) $(TEST_TARGET) 
	./$(TEST_TARGET)

# Build the executable, and run with a simple parameter list
exe :	$(LIB_FILES) $(PROGS)
	./$(PROGS) -t 3 testdir/

.c.o :
	$(CC) $(CFLAGS) -c $<
.cpp.o :
	$(CPP) $(CPPFLAGS) -c $<

ssfi : $(LIB_FILES)
	$(CPP) $(CPPFLAGS) $(LINK_FLAGS) -o ssfi $(LIB_FILES)

#test: $(TEST_TARGET)
test: run_test
.PHONY: test

docs: doc/html
.PHONY: docs

doc/html: $(SRCS) $(UNIT_TEST_FILE)
	@mkdir doc
	$(DOXYGEN_BIN) Doxyfile

clean_docs:
	@rm -rf doc

$(TEST_TARGET): $(UNITTEST_SRC_FILES)
	$(CPP) -g -pthread $(INC_DIRS) -DTEST $(UNITTEST_SRC_FILES) -o $(TEST_TARGET)

run_test: $(TEST_TARGET)
	./$(TEST_TARGET)

gdb_test: $(TEST_TARGET)
	$(GDB_BIN) ./$(TEST_TARGET)

# Rule to generate runner file automatically
$(UNIT_TEST_AUTOGEN_RUNNER): $(UNIT_TEST_FILE)
	ruby unity/auto/generate_test_runner.rb $(UNIT_TEST_FILE) $(UNIT_TEST_AUTOGEN_RUNNER)

clean_buildprods:
	rm -f $(CLEANFILES) $(PROGS)

clean:  clean_buildprods clean_docs
.PHONY: clean
