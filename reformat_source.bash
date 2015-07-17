#!/bin/bash
# LAST MODIFICATION: "Fri, 17 Jul 2015 14:28:10 ()"
# (C) 2015 by Douglas L. Potts, <pottsdl@gmail.com>
#
# ==============================================================================
# ==============================================================================
# Copyright (c) 2015 Douglas Lee Potts
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#     Unless required by applicable law or agreed to in writing, software
#     distributed under the License is distributed on an "AS IS" BASIS,
#     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#     See the License for the specific language governing permissions and
#     limitations under the License.
# ==============================================================================
# ==============================================================================
#
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
