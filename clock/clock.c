#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "lib44780.h"

int main(int argc, char **argv)
{
    struct Lcd lcd;
    openLcd(&lcd, "gpiochip0", 7, 8, 25, 24, 23, 18);
    initLcd(&lcd);

    time_t t = time(NULL);
    struct tm local;
    localtime_r(&t, &local);
    int seconds = local.tm_sec;
    int minutes = local.tm_min;
    int hours = local.tm_hour;
    int yday = local.tm_yday;
    char date[16];
    char hour[16];
    char minute[16];
    char second[16];
    char pad[16];

    strftime(date, sizeof(date), " %Y/%m/%d %a", &local);
    printLcd(&lcd, date);
    gotoLine(&lcd, 1);
    if (hours < 10)
    {
        sprintf(pad, "     ");
    }
    else
    {
        sprintf(pad, "    ");
    }
    printLcd(&lcd, pad);
    strftime(hour, sizeof(hour), "%-H", &local);
    printLcd(&lcd, hour);
    strftime(minute, sizeof(minute), ":%M", &local);
    printLcd(&lcd, minute);
    strftime(second, sizeof(second), ":%S", &local);
    printLcd(&lcd, second);

    while (1)
    {
        t = time(NULL);
        localtime_r(&t, &local);
        if (yday != local.tm_yday)
        {
            yday = local.tm_yday;
            setCursor(&lcd, 0, 0);
            strftime(date, sizeof(date), " %Y/%m/%d %a", &local);
            printLcd(&lcd, date);
        }
        if (hours != local.tm_hour)
        {
            hours = local.tm_hour;
            gotoLine(&lcd, 1);
            if (hours < 10)
            {
                sprintf(pad, "     ");
            }
            else
            {
                sprintf(pad, "    ");
            }
            printLcd(&lcd, pad);
            strftime(hour, sizeof(hour), "%-H", &local);
            printLcd(&lcd, hour);
        }
        if (minutes != local.tm_min)
        {
            minutes = local.tm_min;
            setCursor(&lcd, 1, 6);
            strftime(minute, sizeof(minute), ":%M", &local);
            printLcd(&lcd, minute);
        }
        if (seconds != local.tm_sec)
        {
            seconds = local.tm_sec;
            setCursor(&lcd, 1, 9);
            strftime(second, sizeof(second), ":%S", &local);
            printLcd(&lcd, second);
        }
        usleep(100 * 1000);
    }
}
