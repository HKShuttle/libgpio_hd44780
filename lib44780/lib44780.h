#ifndef LIB44780_H
#define LIB44780_H

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

int openLcd(struct Lcd *lcd, char *chipname, int rs, int en, int d4, int d5, int d6, int d7);

int closeLcd(struct Lcd *lcd);

void ndelay(int nanosec);

void sendNibble(struct Lcd *lcd, int rs, int d7, int d6, int d5, int d4);

void sendByte(struct Lcd *lcd, int rs, int byte);

void sendCommand(struct Lcd *lcd, int byte);

void sendData(struct Lcd *lcd, int byte);

void clearLcd(struct Lcd *lcd);

void setCursor(struct Lcd *lcd, int nextLine, int column);

void gotoLine(struct Lcd *lcd, int nextLine);

void printLcd(struct Lcd *lcd, char *text);

void initLcd(struct Lcd *lcd);

#endif
