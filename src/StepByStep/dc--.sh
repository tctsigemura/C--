#!/bin/sh
./lx
./sn STDIN.lx
./fsm STDIN.sn
./gsm STDIN.fsm
./op STDIN.sm
./vm STDIN.op
./tac STDIN.vm

rm *.nt *.lx *.sn *.fsm *.sm *.op *.vm
