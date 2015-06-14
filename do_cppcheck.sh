#!/bin/sh
# FILE: "/home/dpotts/proto/search/do_cppcheck.sh" {{{
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
