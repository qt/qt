#!/bin/sh
for a in `cat tests`; do ./tst_visual -testdir $a; done
