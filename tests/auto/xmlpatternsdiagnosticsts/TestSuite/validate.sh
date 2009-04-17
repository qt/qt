#!/usr/bin/env bash
set p4Where `p4 where //depot/autotests/4.4/tests/auto/xmlpatternsxqts/XQTS/XQTSCatalog.xsd`
xmllint --xinclude --noout --schema $4 DiagnosticsCatalog.xml
