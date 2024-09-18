#!/bin/bash

function should_succ_on_file() {
  FILE=$1
  ./run.sh $FILE &> /tmp/lartc.test.succ.$FILE.log
  ESIT=$?
  if [ $ESIT == 0 ]; then
    echo -e " - $FILE ... Ok"
  else
    echo -e " - $FILE ... Err"
  fi
}

function should_fail_on_file() {
  FILE=$1
  ./run.sh $FILE &> /tmp/lartc.test.fail.$FILE.log
  ESIT=$?
  if [ $ESIT == 0 ]; then
    echo -e " - $FILE ... Err"
  else
    echo -e " - $FILE ... Ok"
  fi
}

should_fail_on_file bad-decl-type-checking
should_fail_on_file bad-name-resolution
should_fail_on_file bad-syntax
should_fail_on_file bad-type-checking
should_fail_on_file bad-duplicate-definition
should_succ_on_file hello
should_succ_on_file tree
should_succ_on_file chatgpt
