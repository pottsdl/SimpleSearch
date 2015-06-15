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

DIVIDER_STR := "+-------------------------------------------------------------------+"

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

all :	$(LIB_FILES) $(PROGS) 
.PHONY: all

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
	$(CPP) -g -pthread --coverage -ftest-coverage $(INC_DIRS) -DTEST $(UNITTEST_SRC_FILES) -o $(TEST_TARGET)

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

coverage: output
.PHONY: coverage

tags: $(UNIT_TEST_FILE) $(SRCS) $(UNITTEST_SRC_FILES)
	ctags -R .

cscope.out: $(UNIT_TEST_FILE) $(SRCS) $(UNITTEST_SRC_FILES)
	cscope -Rb

clean_coverage:
	rm -rf *.o *.bb *.bbg *.da *.gcno *.gcda *.info output example \
	descriptions


output: $(PROGS) $(TEST_TARGET) descriptions test_3thread_testdir test_singlethread_testdir test_singlethread_notdir test_singlethread_verbose_testdir test_unittest
	@echo
	@echo '*'
	@echo '* Generating HTML output'
	@echo '*'
	@echo
	$(GENHTML) trace_3thread.info trace_singlethread.info trace_singlethread_notdir.info trace_singlethread_verbose.info trace_unittest.info \
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
	$(LCOV) --capture --directory . --output-file trace_3thread.info --test-name test_3thread_testdir --no-external

test_singlethread_testdir:
	@echo
	@echo '*'
	@echo '* Test case 2: running ./ssfi -t 1 testdir' 
	@echo '*'
	@echo
	$(LCOV) --zerocounters --directory .
	./$(PROGS) -t 1 testdir/
	$(LCOV) --capture --directory . --output-file trace_singlethread.info --test-name test_singlethread_testdir --no-external

test_singlethread_notdir:
	@echo
	@echo '*'
	@echo '* Test case 3: running ./ssfi -t 1 testdir/myfile' 
	@echo '*'
	@echo
	$(LCOV) --zerocounters --directory .
	-./$(PROGS) -t 1 testdir/myfile
	$(LCOV) --capture --directory . --output-file trace_singlethread_notdir.info --test-name test_singlethread_notdir --no-external

test_singlethread_verbose_testdir:
	@echo
	@echo '*'
	@echo '* Test case 4: running ./ssfi -t 1 -v testdir' 
	@echo '*'
	@echo
	$(LCOV) --zerocounters --directory .
	./$(PROGS) -t 1 -v testdir/
	$(LCOV) --capture --directory . --output-file trace_singlethread_verbose.info --test-name test_singlethread_verbose_testdir --no-external

test_unittest:
	@echo
	@echo '*'
	@echo '* Test case 4: running ./test1.out' 
	@echo '*'
	@echo
	$(LCOV) --zerocounters --directory .
	./$(TEST_TARGET)
	$(LCOV) --capture --directory . --output-file trace_unittest.info --test-name test_unittest --no-external

clean_buildprods:
	rm -f $(CLEANFILES) $(PROGS)

clean:  clean_buildprods clean_docs clean_sloc clean_cppcheck clean_coverage
.PHONY: clean

world: exe test memcheck docs sloc cppcheck coverage
.PHONY: world

help:
	@echo $(DIVIDER_STR)
	@echo "| Help file for Super Simple File Indexer project"
	@echo $(DIVIDER_STR)
	@echo "| Targets supported:"
	@echo "|  "
	@echo "|     DEFAULT      $(.DEFAULT_GOAL)"
	@echo "|     all          Builds the ssfi program"
	@echo "|     clean        Cleans the build artifacts"
	@echo "|     help         This help menu"
	@echo "|     world        Build 'ssfi' and all associated doc, check, test, etc. targets"
	@echo "|     exe          Build 'ssfi' and execute with 3 threads on testdir dat directory."
	@echo "|     memcheck     Run valgrind memcheck tool over ssfi, and open the report in EDITOR"
	@echo "|     docs         Run doxygen tool over the source code, output is in:"
	@echo "|                  output is in: `pwd`/doc/html/index.html"
	@echo "|     test         Build and run the unit tests."
	@echo "|     gdb_test     Build and run the unit tests in gdb."
	@echo "|     sloc         Generate a Source Lines of Code (SLoC) report, output is in: sloc_count.txt"
	@echo "|     cppcheck     Run cppcheck, a lint-like static analysis of source code, output is in: cppcheck.txt"
	@echo "|     coverage     Build and run 'ssfi' and unit tests tracking code coverage,"
	@echo "|                  output is in: `pwd`/output/index.html"
	@echo $(DIVIDER_STR)
	@echo "| Example invocations:"
	@echo "|  "
	@echo "|     make"
	@echo "|                Makes the default target (all) with the default options"
	@echo "|     make exe"
	@echo "|                Makes the default target (all) and runs a test instance"
	@echo $(DIVIDER_STR)
.PHONY: help
	
.DEFAULT_GOAL = all
