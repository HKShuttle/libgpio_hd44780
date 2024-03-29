#include <stdio.h>
#include <unistd.h>
#include "./lib44780.h"

int main(int argc, char **argv)
{
	struct Lcd *lcd;
	openLcd(lcd, "gpiochip0", 7, 8, 25, 24, 23, 18);
	initLcd(lcd);
	char txt[32] = "Hello World!\nHD44780 Char LCD";	
	closeLcd(lcd);
	return 0;
}
