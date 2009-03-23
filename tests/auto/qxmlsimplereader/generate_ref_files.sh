#! /bin/bash

for k in `find xmldocs -name \*.xml`; do echo $k...; ./parser/parser $k; done
