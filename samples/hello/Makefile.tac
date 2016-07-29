#
# Makefile.tac : C--言語からTacの実行形式に変換する手順
#

hello.exe: hello.cmm
	cm2e -o hello hello.cmm

clean:
	rm -f hello.exe hello.map
