# c_cmdarguments_parser

# 実行ファイルの作成
- サンプルプログラム
  - `make all`
  - make実行ディレクトリに`parser`が出力されます
- 静的ライブラリ
  - `make library`
  - make実行ディレクトリに`libparser.a`が出力されます

# ライブラリの使い方
- ヘッダ
  - `parser.h`
- 実行ファイルの作り方
  - `gcc -L/usr/local/lib/c_cmdarguments_parser -I/usr/local/lib/c_cmdarguments_parser -o program main.c -lparser`
