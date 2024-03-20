#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#define APP "libgpio_hd44780"

struct Pinout
{
	struct gpiod_line *lcd_rs; // GPIO Pin  7
	struct gpiod_line *lcd_en; // GPIO Pin  8
	struct gpiod_line *lcd_d4; // GPIO Pin 25
	struct gpiod_line *lcd_d5; // GPIO Pin 24
	struct gpiod_line *lcd_d6; // GPIO Pin 23
	struct gpiod_line *lcd_d7; // GPIO Pin 18
};

void initDisplay(struct Pinout pin);
void sendByte(struct Pinout pin, int rs, int byte);

int main(int argc, char **argv)
{
	char *chipname = "gpiochip0";
	struct Pinout pin;
	unsigned int lcd_columns = 16;
	unsigned int lcd_rows = 2;
	struct gpiod_chip *chip;

	chip = gpiod_chip_open_by_name(chipname);

	pin.lcd_rs = gpiod_chip_get_line(chip, 7);
	pin.lcd_en = gpiod_chip_get_line(chip, 8);
	pin.lcd_d4 = gpiod_chip_get_line(chip, 25);
	pin.lcd_d5 = gpiod_chip_get_line(chip, 24);
	pin.lcd_d6 = gpiod_chip_get_line(chip, 23);
	pin.lcd_d7 = gpiod_chip_get_line(chip, 18);

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
	sendByte(pin, 0, 0b00001101); // turn on display
	sendByte(pin, 0, 1); // clear display
	usleep(164 * 2 * 10);
	sendByte(pin, 0, 0b00000110); // set entry mode
}
