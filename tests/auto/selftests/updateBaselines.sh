# Be *really* sure that your fix is right, before running this script.

tests="                 \
       cmptest          \
       cmptest          \
       cmptest          \
       datatable        \
       datetime         \
       expectfail       \
       expectfail       \
       globaldata       \
       globaldata       \
       maxwarnings      \
       multiexec        \
       singleskip       \
       qexecstringlist  \
       differentexec    \
       skip             \
       skip             \
       skipglobal       \
       sleep            \
       strcmp           \
       subtest          \
       warnings"

for test in $tests; do
    echo "Updating $test"
    cd $test
    baseline="../expected_"$test".txt"
    p4 edit $baseline
    ./tst_$test > ../expected_"$test".txt
    p4 revert -a $baseline
    cd .. 
done
