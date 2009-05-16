#!/bin/sh
# Runs all the tests specific to QtXmlPatterns in one go.

# If you add a test, remember to update ./auto.pro too.
# checkxmlfiles is not part of Patternist, but since it shares test code
# and use Patternist, we include it as well.
#
# Sorted alphabetically, with the difference that xmlpatternsxqts appears
# before xmlpattersview, xmlpatternsdiagnosticsts, and xmlpatternsxslts, since
# they have the former as dependency.
tests="                     \
checkxmlfiles               \
patternistexamplefiletree   \
patternistexamples          \
patternistheaders           \
qabstractmessagehandler     \
qabstracturiresolver        \
qabstractxmlforwarditerator \
qabstractxmlnodemodel       \
qabstractxmlreceiver        \
qapplicationargumentparser  \
qautoptr                    \
qsimplexmlnodemodel         \
qsourcelocation             \
qtokenautomaton             \
qxmlformatter               \
qxmlitem                    \
qxmlname                    \
qxmlnamepool                \
qxmlnodemodelindex          \
qxmlquery                   \
qxmlresultitems             \
qxmlserializer              \
xmlpatterns                 \
xmlpatternsxqts             \
xmlpatternsdiagnosticsts    \
xmlpatternsschema           \
xmlpatternsschemats         \
xmlpatternsview             \
xmlpatternsxslts"

os=`uname`

for test in $tests; do
    cd $test
    make distclean
    qmake
    make

    if [ $os = "Darwin" ]; then
        ./tst_"$test".app/Contents/MacOS/tst_"$test"
    else
        ./tst_$test
    fi

    if [ $? -ne 0 ]; then
        exit $?
    else
        cd ..
    fi
done
