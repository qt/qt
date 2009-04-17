#!/bin/sh
qmake
make
cat licenseheader.txt > ../keywords.cpp
cat licenseheader.txt > ../ppkeywords.cpp
./generate_keywords >> ../keywords.cpp
./generate_keywords preprocessor >> ../ppkeywords.cpp
