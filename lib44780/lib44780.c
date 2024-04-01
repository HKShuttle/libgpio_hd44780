#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#ifndef APP
#define APP "libgpio_hd44780"
#endif

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

int openLcd(struct Lcd *lcd, char *chipname, int rs, int en, int d4, int d5, int d6, int d7)
{
	lcd->chipname = chipname;
	lcd->chip = gpiod_chip_open_by_name(lcd->chipname);
	if (!lcd->chip)
	{
		return 1;
	}
	// WIP: to catch gpiod_chip_get_line() error
	lcd->rs = gpiod_chip_get_line(lcd->chip, rs);
	lcd->en = gpiod_chip_get_line(lcd->chip, en);
	lcd->d4 = gpiod_chip_get_line(lcd->chip, d4);
	lcd->d5 = gpiod_chip_get_line(lcd->chip, d5);
	lcd->d6 = gpiod_chip_get_line(lcd->chip, d6);
	lcd->d7 = gpiod_chip_get_line(lcd->chip, d7);

	gpiod_line_request_output(lcd->rs, APP, 0);
	gpiod_line_request_output(lcd->en, APP, 0);
	gpiod_line_request_output(lcd->d4, APP, 0);
	gpiod_line_request_output(lcd->d5, APP, 0);
	gpiod_line_request_output(lcd->d6, APP, 0);
	gpiod_line_request_output(lcd->d7, APP, 0);

	return 0;
}

int closeLcd(struct Lcd *lcd)
{
	gpiod_line_release(lcd->rs);
	gpiod_line_release(lcd->en);
	gpiod_line_release(lcd->d4);
	gpiod_line_release(lcd->d5);
	gpiod_line_release(lcd->d6);
	gpiod_line_release(lcd->d7);
	gpiod_chip_close(lcd->chip);
	return 0;
}

void ndelay(int nanosec)
{
	struct timespec req;
	req.tv_nsec = nanosec;
	req.tv_sec = 0;
	nanosleep(&req, NULL);
}

void sendNibble(struct Lcd *lcd, int rs, int d7, int d6, int d5, int d4)
{
	gpiod_line_set_value(lcd->rs, rs);
	ndelay(60 * 2);
	gpiod_line_set_value(lcd->en, 1);
	gpiod_line_set_value(lcd->d4, d4);
	gpiod_line_set_value(lcd->d5, d5);
	gpiod_line_set_value(lcd->d6, d6);
	gpiod_line_set_value(lcd->d7, d7);
	ndelay(450 * 2);
	gpiod_line_set_value(lcd->en, 0);
	ndelay(1200 * 2);
}

void sendByte(struct Lcd *lcd, int rs, int byte)
{
	int data[8]; // data[0]=LSB, data[7]=MSB
	for (int i = 0; i <= 7; i++)
	{
		data[i] = byte & 1;
		byte >>= 1;
	}

	sendNibble(lcd, rs, data[7], data[6], data[5], data[4]);
	sendNibble(lcd, rs, data[3], data[2], data[1], data[0]);
}

void sendCommand(struct Lcd *lcd, int byte)
{
	sendByte(lcd, 0, byte);
}

void sendData(struct Lcd *lcd, int byte)
{
	sendByte(lcd, 1, byte);
}

void clearLcd(struct Lcd *lcd)
{
	sendCommand(lcd, 1);
	usleep(164 * 2 * 10);
}

void setCursor(struct Lcd *lcd, int nextLine, int column)
{
	const int SET_LINE = 0b10000000;
	switch (nextLine)
	{
	case 0:
		sendCommand(lcd, SET_LINE | column);
		return;
	case 1:
		sendCommand(lcd, SET_LINE | 0x40 | column);
		return;
	default:
		return;
		// todo: implement case2, case3
	}
}

void gotoLine(struct Lcd *lcd, int nextLine)
{
	setCursor(lcd, nextLine, 0);
}

void printLcd(struct Lcd *lcd, char *text)
{
	int i = 0;
	int line = 0;
	while (1)
	{
		if (*(text + i) == '\0')
		{
			return;
		}
		if (*(text + i) == '\n')
		{
			gotoLine(lcd, ++line);
		}
		else
		{
			sendData(lcd, *(text + i));
		}
		i++;
	}
}

void initLcd(struct Lcd *lcd)
{
	usleep(40 * 1000);
	sendNibble(lcd, 0, 0, 0, 1, 1);
	usleep(8 * 1000);
	sendNibble(lcd, 0, 0, 0, 1, 1);
	usleep(200);
	sendNibble(lcd, 0, 0, 0, 1, 1); // detarmined 8-bit mode
	sendNibble(lcd, 0, 0, 0, 1, 0); // set to 4-bit mode
	sendCommand(lcd, 0b00101000);	// function set
	sendCommand(lcd, 0b00001100);	// turn on display
	clearLcd(lcd);
	sendCommand(lcd, 0b00000110); // set entry mode
}
