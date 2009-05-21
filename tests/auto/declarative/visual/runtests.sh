#!/bin/sh
export QFX_USE_SIMPLECANVAS=1
for a in `cat tests`; do ./tst_visual -testdir $a; done
