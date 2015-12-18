#ifndef __DS3234_RTC_H
#define __DS3234_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

void ds3234_init(void);
void ds3234_gettime(void);
void ds3234_settime_string(char* dtstr);
int ds3234_set_datetime(int d, int mo, int y, int h, int mi, int s);

#ifdef __cplusplus
}
#endif

#endif /* __DS3234_RTC_H */
