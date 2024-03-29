# lib44780.h

lib44780.hは、HD44780互換のキャラクタ表示デバイスを制御するためのライブラリです。
本ライブラリは、libgpiodに依存して動作します。libgpiodが動作するLinuxディストリビューションで、本ライブラリを使用したプログラムを実行できます。

本ライブラリは、HD44780互換のキャラクタ表示デバイスを4ビットモードのパラレル出力でのみ制御できます。8ビットパラレル、およびI2CやSPIなどを用いたシリアル出力には対応していません。

# 構造体と関数

本ライブラリで使用される構造体と関数について説明します。

## 構造体

本ライブラリでは、1つの構造体を定義しています。

```
struct Lcd
{
	struct gpiod_line *rs;
	struct gpiod_line *en;
	struct gpiod_line *d4;
	struct gpiod_line *d5;
	struct gpiod_line *d6;
	struct gpiod_line *d7;
	struct gpiod_chip *chip;
	char *chipname;
};
```

`struct Lcd` は、キャラクタ表示デバイスのハンドルを格納する構造体です。1つの `struct gpiod_chip` 、6本のパラレル出力ピンに対応する `struct gpiod_line` 、gpiod_chipの名称を表す文字列を格納します。

構造体 `struct gpiod_line` 、 `struct gpiod_chip` はlibgpiod.hにおいて定義されています。

## 関数

本ライブラリでは、11個の関数を定義しています。

`int openLcd(struct Lcd *lcd, char *chipname, int rs, int en, int d4, int d5, int d6, int d7);`

構造体 `lcd` を初期化し、GPIOピンを出力として設定します。引数には、呼び出し元関数で宣言した構造体 `lcd` 、GPIOチップ名、 rs, en, d4, d5, d6, d7の各出力ピンを接続しているGPIO番号を指定します。

`int closeLcd(struct Lcd *lcd);`

使用したGPIOリソースを開放します。プログラムを終了する前に必ずこの関数を実行してください。

`void ndelay(int nanosec);`

`nanosleep()` 関数のラッパーです。 `nanosec` で指定したナノ秒間、処理を中断します。本ライブラリ内のソースコードの記述を簡略化するために定義しています。

`void sendNibble(struct Lcd *lcd, int rs, int d7, int d6, int d5, int d4);`

キャラクタ表示デバイスに対して、4ビットのデータとrs（レジスタ選択）信号を送信します。
通常、キャラクタ表示デバイスのワード長は8ビットです。半端なデータを送信すると意図しない動作を引き起こす可能性があります。特別な理由がない限り、sendByte()または他のラッパー関数を使用してキャラクタ表示デバイスにアクセスしてください。

`void sendByte(struct Lcd *lcd, int rs, int byte);`

キャラクタ表示デバイスに対して、8ビットのデータとrs（レジスタ選択）信号を送信します。

`void sendCommand(struct Lcd *lcd, int byte);`

`sendByte()` 関数のラッパーです。キャラクタ表示デバイスに対して、8ビットのデータからなるコマンドを送信します。

`void sendData(struct Lcd *lcd, int byte);`

`sendByte()` 関数のラッパーです。キャラクタ表示デバイスに対して、8ビットのデータを送信します。CGRAMまたはDDRAMへのアクセスに使用します。

`void clearLcd(struct Lcd *lcd);`

キャラクタ表示デバイスの表示をすべて消去します。

`void gotoLine(struct Lcd *lcd, int nextLine);`

キャラクタ表示デバイスのカーソルを、 `nextLine` 行目の先頭に移動させます。0行目が先頭の行に対応します。
現在、2行までのデバイスに対応しています。

`void printLcd(struct Lcd *lcd, char *text);`

キャラクタ表示デバイスに対して、文字列 `text` を表示します。ASCII文字を表示できます。
改行文字 `\n` を検出すると改行し、ヌル文字 `\0` を検出すると終了します。

`void initLcd(struct Lcd *lcd);`

キャラクタ表示デバイスを4ビットモードで初期化し、文字列を表示できる状態にします。
