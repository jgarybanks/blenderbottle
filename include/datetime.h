#ifndef __DATETIME_H
#define __DATETIME_H

#include <time.h>

struct datetime
{
	int date;
	int time;
};

int set_tz(int, int, int);
int reset_tz(void);
time_t set_datetime(struct datetime);
time_t set_curr_datetime(void);
time_t set_yyyymmdd_datetime(char *);
struct datetime get_datetime(time_t, int);
time_t add_months(time_t, int);
time_t add_years(time_t, int);
time_t add_days(time_t, int);
time_t add_seconds(time_t, int);
time_t add_minutes(time_t, int);
time_t add_hours(time_t, int);
time_t get_month_start(time_t);
time_t get_month_end(time_t);
time_t get_week_start(time_t);
time_t get_week_end(time_t);
char *fmt_dttime(time_t, char *, char *);
int get_dow(time_t);
int get_isdst(time_t);

#endif /* __DATETIME_H */
