# Programing Laguage C--
Programing Language C-- is an Educational System Description Language.
---
プログラミング言語C−-は、教育用のシステム記述言語です。
C言語をお手本にしていますが、型チェックが厳しい、ポインターがない等、初心者に使いやすい仕様になっています。
配列や構造体はJavaのように全て参照型になります。
C言語で悩んだ配列の実態、配列名、配列を指すポインタの混乱、
多次元配列とポインタ配列の使い分けのようなものはありません。
常に参照です。

詳細は
[マニュアル](https://github.com/tctsigemura/C--/blob/master/doc/cmm.pdf)をご覧ください。

[TacOS](https://github.com/tctsigemura/TacOS/)は
C--言語で記述されています。
C--言語で記述したC−-コンパイラのバージョンも存在します。
C--言語は、ある程度実用的なシステム記述言語です。

## ターゲット
[TacOS](https://github.com/tctsigemura/TacOS/)の
アプリケーションを開発するためのコンパイラと、
C言語を出力するトランスレータが準備してあります。

### コンパイラ
[TacOS](https://github.com/tctsigemura/TacOS/)の
アプリケーション用にTaCのアセンブリ言語を出力します。
[Util--](https://github.com/tctsigemura/Util--)の
ツールを使用してTacOSの実行形式に変換できます。

### トランスレータ
C--言語プログラムをC言語に変換します。
C--言語コンパイラをOSX上で動作させた実績があります。
