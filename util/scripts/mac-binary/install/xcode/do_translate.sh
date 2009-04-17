#!/bin/sh

dir=`dirname "$2/$1"`
mkdir -p "$dir"
sed -e "s,\\\$VERSION_MAJOR\\\$,$VERSION_MAJOR,g" \
    -e "s,\\\$VERSION_MINOR\\\$,$VERSION_MINOR,g" \
    -e "s,\\\$VERSION_PATCH\\\$,$VERSION_PATCH,g" "$1" >"$2/$1"


