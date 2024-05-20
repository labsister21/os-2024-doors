#include <stdint.h>
#include "syscall.h"

int main(void)
{
    uint16_t year, month, day, hour, minute, second;

    char syear[10];
    char smonth[10];
    char sday[10];
    char shour[10];
    char sminute[10];
    char ssecond[10];

    while (true)
    {
        read_rtc(&year, &month, &day, &hour, &minute, &second);
        hour = (hour + 7) % 24;
        itoa((int32_t)year, syear);
        itoa((int32_t)month, smonth);
        itoa((int32_t)day, sday);
        itoa((int32_t)hour, shour);
        itoa((int32_t)minute, sminute);
        itoa((int32_t)second, ssecond);

        // print hour
        if (hour < 10)
        {
            put_char_position(24, 72, '0');
            put_char_position(24, 73, shour[0]);
        }
        else
        {
            put_char_position(24, 72, shour[0]);
            put_char_position(24, 73, shour[1]);
        }
        put_char_position(24, 74, ':');

        // print minute
        if (minute < 10)
        {
            put_char_position(24, 75, '0');
            put_char_position(24, 76, sminute[0]);
        }
        else
        {
            put_char_position(24, 75, sminute[0]);
            put_char_position(24, 76, sminute[1]);
        }
        put_char_position(24, 77, ':');

        // print second
        if (second < 10)
        {
            put_char_position(24, 78, '0');
            put_char_position(24, 79, ssecond[0]);
        }
        else
        {
            put_char_position(24, 78, ssecond[0]);
            put_char_position(24, 79, ssecond[1]);
        }

        // sleep();
    }
    return 0;
}