#!/bin/bash
set -e

DEFAULT="hello"
NAME=$1

if [ -z "$NAME" ]; then
  NAME=$DEFAULT
fi

file="../lart-examples/$NAME.lart"
echo "./builddir/lartc $file"
./builddir/lartc -c $file
