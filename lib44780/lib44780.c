#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#define APP "libgpio_hd44780"

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

int openLcd(struct Lcd lcd, char[] chipname, int rs, int en, int d4, int d5, int d6, int d7){
	lcd.chipname = &chipname;
	lcd.chip = gpiod_chip_open_by_name(lcd.chipname);
	if(!lcd.chip){
		return 1;
	}
	// WIP: to catch gpiod_chip_get_line() error
	lcd.rs = gpiod_chip_get_line(lcd.chip, rs);
	lcd.en = gpiod_chip_get_line(lcd.chip, en);
	lcd.d4 = gpiod_chip_get_line(lcd.chip, d4);
	lcd.d5 = gpiod_chip_get_line(lcd.chip, d5);
	lcd.d6 = gpiod_chip_get_line(lcd.chip, d6);
	lcd.d7 = gpiod_chip_get_line(lcd.chip, d7);
}

int closeLcd(){

}

int main(int argc, char **argv)
{
	struct Pinout pin;
	struct gpiod_chip *chip;

	gpiod_line_request_output(pin.lcd_rs, APP, 0);
	gpiod_line_request_output(pin.lcd_en, APP, 0);
	gpiod_line_request_output(pin.lcd_d4, APP, 0);
	gpiod_line_request_output(pin.lcd_d5, APP, 0);
	gpiod_line_request_output(pin.lcd_d6, APP, 0);
	gpiod_line_request_output(pin.lcd_d7, APP, 0);

	initDisplay(pin);
	sendByte(pin, 0, 0b10000000);
	char txt[32] = "Hello World!\nHD44780 Char LCD";	
	for (int i = 0; i < 32; i++)
	{
		if (txt[i] == '\0')
		{
			break;
		}
		if (txt[i] == '\n')
		{
			sendByte(pin, 0, 0b10000000 | 0x40); // go to next row
			continue;
		}
		sendByte(pin, 1, txt[i]);
	}
	
	gpiod_line_release(pin.lcd_rs);
	gpiod_line_release(pin.lcd_en);
	gpiod_line_release(pin.lcd_d4);
	gpiod_line_release(pin.lcd_d5);
	gpiod_line_release(pin.lcd_d6);
	gpiod_line_release(pin.lcd_d7);
	gpiod_chip_close(chip);
	return 0;
}

void ndelay(int nanosec)
{
	struct timespec req;
	req.tv_nsec = nanosec;
	req.tv_sec = 0;
	nanosleep(&req, NULL);	
}

void sendNibble(struct Pinout pin, int rs, int d7, int d6, int d5, int d4)
{
	gpiod_line_set_value(pin.lcd_rs, rs);
	ndelay(60 * 2);
	gpiod_line_set_value(pin.lcd_en, 1);
	gpiod_line_set_value(pin.lcd_d4, d4);
	gpiod_line_set_value(pin.lcd_d5, d5);
	gpiod_line_set_value(pin.lcd_d6, d6);
	gpiod_line_set_value(pin.lcd_d7, d7);
	ndelay(450 * 2);
	gpiod_line_set_value(pin.lcd_en, 0);
	ndelay(1200 * 2);
}

void sendByte(struct Pinout pin, int rs, int byte)
{
	int data[8]; // data[0]=LSB, data[7]=MSB
	for (int i = 0; i <= 7; i++)
	{
		data[i] = byte & 1;
		byte >>= 1;
	}
	
	sendNibble(pin, rs, data[7], data[6], data[5], data[4]);
	sendNibble(pin, rs, data[3], data[2], data[1], data[0]);	
}

void initDisplay(struct Pinout pin)
{
	usleep(40 * 1000);
	sendNibble(pin, 0, 0, 0, 1, 1);
	usleep(8 * 1000);
	sendNibble(pin, 0, 0, 0, 1, 1);
	usleep(200);
	sendNibble(pin, 0, 0, 0, 1, 1); // detarmined 8-bit mode
	sendNibble(pin, 0, 0, 0, 1, 0); // set to 4-bit mode
	sendByte(pin, 0, 0b00101000); // function set
	sendByte(pin, 0, 0b00001100); // turn on display
	sendByte(pin, 0, 1); // clear display
	usleep(164 * 2 * 10);
	sendByte(pin, 0, 0b00000110); // set entry mode
}
