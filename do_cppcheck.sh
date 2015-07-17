#!/bin/sh
# FILE: "C:\Users\pottsdo\src\search\do_cppcheck.sh" {{{
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

TEMPLATE_TYPE=${TEMPLATE_TYPE:=gcc}
NUM_SIMULTANEOUS=${NUM_SIMULTANEOUS:=1}
EXTRA_PARAMS=${EXTRA_PARAMS:=}
INCLUDES_FILE=${INCLUDES_FILE:=includes_file.txt}
export CPP_CHECK_OPTIONS=--enable=all
export CPP_CHECK_OPTIONS="$CPP_CHECK_OPTIONS --template=$TEMPLATE_TYPE"
export CPP_CHECK_OPTIONS="$CPP_CHECK_OPTIONS --includes-file=$INCLUDES_FILE"
export CPP_CHECK_OPTIONS="$CPP_CHECK_OPTIONS -j $NUM_SIMULTANEOUS"
export CPP_CHECK_OPTIONS="$CPP_CHECK_OPTIONS -q"

# Put in any #defines here
export DEFINES=""

for define in $DEFINES; do
  export CPP_CHECK_OPTIONS="$CPP_CHECK_OPTIONS -D${define}"
done

export CPP_CHECK_OPTIONS="$CPP_CHECK_OPTIONS $EXTRA_PARAMS"
echo cppcheck ${CPP_CHECK_OPTIONS} .
if [ $# -lt 1 ]; then
  echo "usage: $0 <dir_root_to_check>"
  exit 1
fi
cppcheck ${CPP_CHECK_OPTIONS} $1
