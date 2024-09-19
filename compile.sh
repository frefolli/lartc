#!/bin/bash

LLVM_IN=tmp/bucket.ll
AS_IN=tmp/bucket.s
LD_IN=tmp/bucket.o
EXE=tmp/bucket.exe

llc $LLVM_IN -o $AS_IN
as $AS_IN -o $LD_IN
"/usr/bin/ld" --hash-style=gnu --build-id --eh-frame-hdr -m elf_x86_64 -dynamic-linker /lib64/ld-linux-x86-64.so.2 -o $EXE /usr/bin/../lib/gcc/x86_64-redhat-linux/14/../../../../lib64/crt1.o /usr/bin/../lib/gcc/x86_64-redhat-linux/14/../../../../lib64/crti.o /usr/bin/../lib/gcc/x86_64-redhat-linux/14/crtbegin.o -L/usr/bin/../lib/clang/18/lib/x86_64-redhat-linux-gnu -L/usr/bin/../lib/gcc/x86_64-redhat-linux/14 -L/usr/bin/../lib/gcc/x86_64-redhat-linux/14/../../../../lib64 -L/lib/../lib64 -L/usr/lib/../lib64 -L/lib -L/usr/lib $LD_IN -lgcc --as-needed -lgcc_s --no-as-needed -lc -lgcc --as-needed -lgcc_s --no-as-needed /usr/bin/../lib/gcc/x86_64-redhat-linux/14/crtend.o /usr/bin/../lib/gcc/x86_64-redhat-linux/14/../../../../lib64/crtn.o
