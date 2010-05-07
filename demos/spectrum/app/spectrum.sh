#!/bin/sh

# Shell script for launching spectrum application on Unix systems other than Mac OSX

bindir=`dirname "$0"`
LD_LIBRARY_PATH="${bindir}:${LD_LIBRARY_PATH}"
export LD_LIBRARY_PATH
exec "${bindir}/spectrum.bin" ${1+"$@"}

