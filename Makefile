CC         = gcc
CPP        = g++

INC_DIRS   = -I. -Iunity
LIBFLAGS   = -L/usr/lib -L /usr/local/lib -L/usr/lib/x86_64-linux-gnu
CFLAGS     = -g --coverage -ftest-coverage -D_REENTRANT -pedantic -Wall -pthread $(LIBFLAGS)
CPPFLAGS   = $(CFLAGS) -ansi
LCOV       = $(shell which lcov)
GENHTML    = $(shell which genhtml)
GENDESC    = $(shell which gendesc)
GENPNG     = $(shell which genpng)
# Depending on the presence of the GD.pm perl module, we can use the
# special option '--frames' for genhtml
USE_GENPNG := $(shell $(GENPNG) --help >/dev/null 2>/dev/null; echo $$?)
ifeq ($(USE_GENPNG),0)
  FRAMES := --frames
else
  FRAMES :=
endif


LINK_FLAGS = -lstdc++
DOXYGEN_BIN = $(shell which doxygen)
GDB_BIN     = $(shell which gdb)
VALGRIND_BIN= $(shell which valgrind)
SLOCCOUNT_BIN = $(shell which sloccount)
SLOC_DATADIR = `pwd`/.slocdata
SLOC_OUTFILE = sloc_count.txt
MEMCHECK_FILE = memcheck.txt
CPPCHECK_BIN = ./do_cppcheck.sh

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

memcheck: $(LIB_FILES) $(PROGS)
	$(VALGRIND_BIN) --tool=memcheck --leak-check=yes --log-file=$(MEMCHECK_FILE) ./$(PROGS) -t 3 testdir/subtestdir/subsubdir
	$(EDITOR) $(MEMCHECK_FILE)

.c.o :
	$(CC) $(CFLAGS) -c $<
.cpp.o :
	$(CPP) $(CPPFLAGS) -c $<

ssfi : $(LIB_FILES)
	$(CPP) $(CPPFLAGS) $(LINK_FLAGS) -o ssfi $(LIB_FILES)

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

sloc: sloc_count.txt
.PHONY: sloc

clean_sloc:
	@rm -rf $(SLOC_DATADIR) $(SLOC_OUTFILE)
.PHONY: clean_sloc

sloc_count.txt:
	@if [ ! -d $(SLOC_DATADIR) ]; then mkdir $(SLOC_DATADIR); fi
	$(SLOCCOUNT_BIN) --duplicates --datadir $(SLOC_DATADIR) --wide --details . 2> /dev/null > $(SLOC_OUTFILE)
	@echo "Successfully built SLOC report, located: $(SLOC_OUTFILE)"

cppcheck.txt: $(UNIT_TEST_FILE) $(SRCS) $(UNITTEST_SRC_FILES)
	$(CPPCHECK_BIN) .   2>&1 | tee cppcheck.txt

clean_cppcheck:
	@rm -ff cppcheck.txt

cppcheck: cppcheck.txt
.PHONY: cppcheck

output: $(PROGS) descriptions test_3thread_testdir test_singlethread_testdir
	@echo
	@echo '*'
	@echo '* Generating HTML output'
	@echo '*'
	@echo
	$(GENHTML) trace_3thread.info trace_singlethread.info \
		   --output-directory output --title "Basic example" \
		   --show-details --description-file descriptions $(FRAMES) \
		   --legend
	@echo
	@echo '*'
	@echo '* See '`pwd`/output/index.html
	@echo '*'
	@echo
descriptions: descriptions.txt
	$(GENDESC) descriptions.txt -o descriptions


test_3thread_testdir:
	@echo
	@echo '*'
	@echo '* Test case 1: running ./ssfi -t 3 testdir'
	@echo '*'
	@echo
	$(LCOV) --zerocounters --directory .
	./$(PROGS) -t 3 testdir/
	$(LCOV) --capture --directory . --output-file trace_3thread.info --test-name test_noargs --no-external

test_singlethread_testdir:
	@echo
	@echo '*'
	@echo '* Test case 2: running ./ssfi -t 3 testdir' 
	@echo '*'
	@echo
	$(LCOV) --zerocounters --directory .
	./$(PROGS) -t 1 testdir/
	$(LCOV) --capture --directory . --output-file trace_singlethread.info --test-name test_noargs --no-external

test_singlethread_verbose_testdir:
	@echo
	@echo '*'
	@echo '* Test case 2: running ./ssfi -t 3 testdir' 
	@echo '*'
	@echo
	$(LCOV) --zerocounters --directory .
	./$(PROGS) -t 1 -v testdir/
	$(LCOV) --capture --directory . --output-file trace_singlethread.info --test-name test_noargs --no-external

clean_buildprods:
	rm -f $(CLEANFILES) $(PROGS)

clean:  clean_buildprods clean_docs clean_sloc clean_cppcheck
.PHONY: clean
