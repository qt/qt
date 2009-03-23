#!/bin/sh

if [ $# -ne 1 ]; then
    echo "You need to pass in one argument: the file path to Baseline.xml which you want the summary for."
    exit 1
fi

# We run dirname, so we can locate summarizeBaseline.xsl regardless of our
# working directory.
xmlpatterns `dirname $0`/summarizeBaseline.xsl $1
