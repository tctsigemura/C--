#!/bin/sh
./lx
./sn STDIN.lx
./sm STDIN.sn
./op STDIN.sm
./vm STDIN.op
./tac STDIN.vm

rm *.nt *.lx *.sn *.sm *.op *.vm
