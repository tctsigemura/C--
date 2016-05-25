#
# C-- 言語から .exe ファイルと a.out ファイルを作る手順
#

all: hello hello.exe hello.v

# UNIX, Mac の a.out へ変換
hello: hello.cmm
	cm2c -o hello hello.cmm

# TacOS の実行形式を作る
hello.exe: hello.cmm
	cm2e -o hello.exe hello.cmm

# C--コンパイラの中間言語に変換する
hello.v: hello.cmm
	cm2v hello.cmm

clean:
	rm -f hello hello.c hello.s hello.v *.lst *.sym *.map *.o *.exe *~
