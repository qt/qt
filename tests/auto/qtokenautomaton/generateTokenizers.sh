#!/bin/bash
tokenizers=`find tokenizers/* -type d`
for tokenizer in $tokenizers; do
    pushd $tokenizer
    basename=`basename $tokenizer`
    xmllint --noout --schema ../../../../../src/xmlpatterns/qtokenautomaton/qtokenautomaton.xsd $basename.xml
    java net.sf.saxon.Transform -xsl:../../../../../src/xmlpatterns/qtokenautomaton/qautomaton2cpp.xsl $basename.xml
    popd
done
