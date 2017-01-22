# Super Simple File Indexer Project ![Build Status](https://travis-ci.org/pottsdl/SimpleSearch.svg?branch=master)
------------------------------------------------------------------------------ 

Package includes Makefile and assorted source files, and some supporting Software
Engineering material (like SLOC report, Source Documentation (Doxgyen), Code
Coverage, Memory usage analysis, and Static Analysis run)


Project is copyrighted (c) by Douglas Potts.


This project is Licensed under the Apache License, Version 2.0.
See the [License](http://www.apache.org/licenses/LICENSE-2.0) at [http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)


## How to Build
To build the executable:

  % make

## Other Targets
There are many other targets for the assorted material referred to above, invoke "make help" to see what those targets are and how to run them.

  % make help


### Help for Super Simple File Indexer project

+-------------------------------------------------------------------+

**Targets supported:**

|     Target       | Description
|  --------------- |:----------------------------------|
|     DEFAULT      | all
|     all          | Builds the ssfi program
|     clean        | Cleans the build artifacts
|     help         | This help menu
|     exe          | Build 'ssfi' and execute with 3 threads on testdir dat directory.
|     memcheck     | Run valgrind memcheck tool over ssfi, and open the report in EDITOR
|     docs         | Run doxygen tool over the source code, output is in:  output is in: /home/dpotts/proto/search_with_git/doc/html/index.html
|     test         | Build and run the unit tests.
|     gdb_test     | Build and run the unit tests in gdb.
|     sloc         | Generate a Source Lines of Code (SLoC) report, output is in: sloc_count.txt
|     cppcheck     | Run cppcheck, a lint-like static analysis of source code, output is in: cppcheck.txt
|     coverage     | Build and run 'ssfi' and unit tests tracking code coverage, output is in:  /home/dpotts/proto/search_with_git/output/index.html


### Example invocations:

```
      make
                 Makes the default target (all) with the default options
      make exe
                 Makes the default target (all) and runs a test instance
```
