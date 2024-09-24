#!/bin/bash

RED_TEXT="\x1b[1;31m"
GREEN_TEXT="\x1b[1;32m"
PURPLE_TEXT="\x1b[1;35m"
AZURE_TEXT="\x1b[1;36m"
NORMAL_TEXT="\x1b[0;39m"

function exec_on() {
  FILE="../lart-examples/$1.lart"
  echo "./builddir/lartc -c $FILE"
  ./builddir/lartc -c $FILE -o /tmp/test.lart.$1.o
}

function esit_ok() {
  FILE=$1
    echo -e " - $FILE ... $GREEN_TEXT" "Ok" "$NORMAL_TEXT"
}

function esit_err() {
  FILE=$1
    echo -e " - $FILE ... $RED_TEXT" "Err" "$NORMAL_TEXT"
}

function should_succ_on_file() {
  FILE=$1
  exec_on $FILE &> /tmp/lartc.test.succ.$FILE.log
  ESIT=$?
  if [ $ESIT == 0 ]; then
    esit_ok $FILE
  else
    esit_err $FILE
  fi
}

function should_fail_on_file() {
  FILE=$1
  exec_on $FILE &> /tmp/lartc.test.fail.$FILE.log
  ESIT=$?
  if [ $ESIT == 0 ]; then
    esit_err $FILE
  else
    esit_ok $FILE
  fi
}

should_fail_on_file bad-decl-type-checking
should_fail_on_file bad-name-resolution
should_fail_on_file bad-syntax
should_fail_on_file bad-type-checking
should_fail_on_file bad-duplicate-definition
should_fail_on_file bad-include-directives
should_succ_on_file hello
should_succ_on_file tree
should_succ_on_file chatgpt
should_succ_on_file include-files
should_succ_on_file array_access
should_succ_on_file strucs-as-rvalue
should_succ_on_file strucs
should_succ_on_file ts
should_succ_on_file function-pointer
