TARGET=.bin/
PROJ=lib44780_test
CC=cc
SRC=test.c
LIBS=-lgpiod lib44780.c

all:
	mkdir $(TARGET) ; $(CC) $(SRC) $(LIBS) -o $(TARGET)$(PROJ)

clean:
	rm $(PROJ)