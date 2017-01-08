#!/bin/sh
cc -E -Wno-comment -std=c99 -nostdinc -DNWORD=16 -DNBYTE=8\
 -I/usr/local/cmmLib -I/usr/local/cmmInclude - < ${1} | ../../../src/c--
