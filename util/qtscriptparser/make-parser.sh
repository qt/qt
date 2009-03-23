#!/bin/sh

me=$(dirname $0)
mkdir -p $me/out
(cd $me/out && ${QLALR-qlalr} --troll --no-lines ../qscript.g)

for f in $me/out/*.{h,cpp}; do
    n=$(basename $f)
    p4 open $me/../../src/script/$n
    cp $f $me/../../src/script/$n
done

p4 revert -a $me/../../src/script/...
p4 diff -du $me/../../src/script/...

