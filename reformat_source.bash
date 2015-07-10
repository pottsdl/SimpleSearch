#!/bin/bash
# FILE: "/home/dpotts/proto/search_with_git/reformat_sourc.bash" {{{
# LAST MODIFICATION: "Mon, 15 Jun 2015 16:19:03 (dpotts)"
# (C) 2015 by Douglas L. Potts, <pottsdl@gmail.com>
# $Id:$ }}}

# Generate the source code list, exclude the unit testing framework cod (Unity)
find . -name '*.c' -o -name '*.h' -o -name '*.cpp' -o -name '*.hpp'|grep -v unity > file_list.lis

# for each file in the list, reformat with output going via stdout to
# <filename>.new, then rm'ing the original, and then moving .new over to the
# original filename.
for fname in `cat file_list.lis`; do
  indent -st $fname > $fname.new;
  rm $fname;
  mv $fname.new $fname;
done
