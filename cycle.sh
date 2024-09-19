#!/bin/bash
RED_TEXT="\x1b[1;31m"
GREEN_TEXT="\x1b[1;32m"
PURPLE_TEXT="\x1b[1;35m"
AZURE_TEXT="\x1b[1;36m"
NORMAL_TEXT="\x1b[0;39m"
set -e

PASS_N=0

function increment_pass() {
  PASS_N=$(($PASS_N+1))
}

function esit_ok() {
  FILE_I=$1
  FILE_O=$2
  echo -e " - $FILE_I == $FILE_O ... $GREEN_TEXT" "Ok" "$NORMAL_TEXT"
}

function esit_err() {
  FILE_I=$1
  FILE_O=$2
  echo -e " - $FILE_I != $FILE_O ... $RED_TEXT" "Err" "$NORMAL_TEXT"
}

function compare() {
  FILE_I=$1
  FILE_O=$2
  if [ -z "$(diff $FILE_I $FILE_O)" ]; then
    esit_ok  $FILE_I $FILE_O
  else
    esit_err $FILE_I $FILE_O
    echo -e "exit on $AZURE_TEXT" "pass $PASS_N" "$NORMAL_TEXT"
    exit 1
  fi
}

LARTC_OUTPUT="/tmp/decl_tree.txt"
TMP_DIR="/tmp/lartc.cycle.d"
PREVIOUS_PASS="$TMP_DIR/pass.lart"

function inizialize() {
  mkdir -p $TMP_DIR
  make run
  cp $LARTC_OUTPUT $PREVIOUS_PASS 
}

function dopass() {
  increment_pass
  ./builddir/lartc $LARTC_OUTPUT
  compare $PREVIOUS_PASS $LARTC_OUTPUT
  cp $LARTC_OUTPUT $PREVIOUS_PASS 
}

inizialize
dopass
dopass
dopass
