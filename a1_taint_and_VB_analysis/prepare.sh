#!/bin/bash
for file in tests/*.c;
do
  clang-7 -fno-discard-value-names -emit-llvm -S -o "${file/.c/.ll}" $file
done

for file in tests/*.ll;
do
  opt-7 -dot-cfg $file;
  mv cfg.main.dot "${file/.ll/}.dot";
done