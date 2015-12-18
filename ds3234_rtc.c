
#include <stdio.h>
#include "nrf_delay.h"
#include "spi_module.h"
#include "nordic_common.h"
#include "binary.h"
#include "ds3234_rtc.h"
#include <stdlib.h>

void ds3234_init(void)
{
    uint8_t command[2] = { 0x8E, 0x60 };
    UNUSED_VARIABLE(spi_transfer(command, 2));
}

void ds3234_gettime(void)
{
    int d, mo, y, h, mi, s;
    int datetime[7]; //second,minute,hour,null,day,month,year
    int n;
    int a, b;
    uint8_t command[2];
    uint8_t * rbuff;

    for (int i = 0; i <= 6; i++) {
        if (i == 3)
            i++;

        command[0] = i + 0x00;
        command[1] = 0x00;
        rbuff = spi_transfer(command, 2);
        n = rbuff[1];

        a = n & B00001111;
        if (i == 2) {
            b = (n & B00110000) >> 4; //24 hour mode
            if (b == B00000010)
                b = 20;
            else if (b == B00000001)
                b = 10;
            datetime[i] = a + b;
        }
        else if (i == 4) {
            int b = (n & B00110000) >> 4;
            datetime[i] = a + b * 10;
        }
        else if (i == 5) {
            b = (n & B00010000) >> 4;
            datetime[i] = a + b * 10;
        }
        else if (i == 6) {
            b = (n & B11110000) >> 4;
            datetime[i] = a + b * 10;
        }
        else {
            b = (n & B01110000) >> 4;
            datetime[i] = a + b * 10;
        }
    }

    s	= datetime[0];
    mi	= datetime[1];
    h	= datetime[2];
    d	= datetime[4];
    mo	= datetime[5];
    y	= datetime[6] + 2000;

    printf("current datetime = [%04d-%02d-%02d %02d:%02d:%02d]\r\n", y, mo, d, h, mi, s);
}


int ds3234_set_datetime(int d, int mo, int y, int h, int mi, int s)
{
    int datetime[7] = {s, mi, h, 0, d, mo, y};
    uint8_t command[2];

    for (int i = 0; i <= 6; i++) {
        if (i == 3)
            i++;
        int b = datetime[i] / 10;
        int a = datetime[i] - b * 10;
        if (i == 2) {
            if (b == 2)
                b = B00000010;
            else if (b == 1)
                b = B00000001;
        }
        datetime[i] = a + (b << 4);

        command[0] = i + 0x80;
        command[1] = datetime[i];
        UNUSED_VARIABLE(spi_transfer(command, 2));
    }

    return 0;
}


int cnv_str_to_int(int* ri, char* src, char sep)
{
    int lindex = 0;
    char numbuf[5];

    while (src[*ri] != sep) {
        numbuf[lindex++] = src[(*ri)++];
    }

    numbuf[lindex] = 0;
    (*ri)++;

    return atoi(numbuf);
}


void ds3234_settime_string(char* dtstr)
{
    int year, month, day, hour, minute, second;

    int rindex = 0;
    year = cnv_str_to_int(&rindex, dtstr, '-') - 2000;
    month = cnv_str_to_int(&rindex, dtstr, '-');
    day = cnv_str_to_int(&rindex, dtstr, ' ');
    hour = cnv_str_to_int(&rindex, dtstr, ':');
    minute = cnv_str_to_int(&rindex, dtstr, ':');
    second = cnv_str_to_int(&rindex, dtstr, ':');

    ds3234_set_datetime(day, month, year, hour, minute, second);
}

