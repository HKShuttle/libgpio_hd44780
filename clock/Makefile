TARGET=.bin/
PROJ=charLCD_clock
CC=cc
SRC=clock.c
LIBS=lib44780.c -lgpiod

all:
	mkdir $(TARGET) ; $(CC) $(SRC) $(LIBS) -o $(TARGET)$(PROJ)

clean:
	rm $(PROJ)
