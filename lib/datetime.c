#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dateTime.h"

static char *abb_days[] =
	{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static char *full_days[] =
	{ "Sunday", "Monday", "Tuesday", "Wednesday",
	  "Thursday", "Friday", "Saturday" };
static char *abb_months[] =
	{ "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char *full_months[] =
	{ "January", "February", "March", "April", "May", "June",
	  "July", "August", "September", "October", "November", "December" };

static char tz_save[20];
static char tz_set_string[20];

int
set_tz(int tz_offset_hr, int tz_offset_min, int tz_daylight)
{
	int i;
	int number_of_zones;

	struct ZI {
		int hour_offset;
		int minute_offset;
		char *st;
		char *dt;
	};

	struct ZI zone_info[] = {
		{ 4,  0, "AST", "ADT"},
		{ 5,  0, "EST", "EDT"},
		{ 6,  0, "CST", "CDT"},
		{ 7,  0, "MST", "MDT"},
		{ 8,  0, "PST", "PDT"},
		{ 3, 30, "NST", "NDT"}
	};

	char tz_offset_min_string[10];
	char tz_daylight_str[10];

	sprintf(tz_save, "TZ=%s", getenv("TZ"));

	number_of_zones = sizeof(zone_info)/sizeof(struct ZI);

	for (i = 0; i < number_of_zones; i++) {
		if (zone_info[i].hour_offset == tz_offset_hr &&
				zone_info[i].minute_offset == tz_offset_min) {
			break;
		}
	}

	/*
	** Defaults to Central
	*/
	if (i == number_of_zones) {
		i = 2;
	}

	if (tz_offset_min == 0) {
		strcpy(tz_offset_min_string, "");
	} else {
		sprintf(tz_offset_min_string, ":%d", tz_offset_min);
	}

	if (tz_daylight == 0) {
		strcpy(tz_daylight_str, "");
	} else {
		sprintf(tz_daylight_str, "%s", zone_info[i].dt);
	}

	sprintf(tz_set_string, "TZ=%s%d%s%s",
		zone_info[i].st,
		tz_offset_hr,
		tz_offset_min_string,
		tz_daylight_str);

	if (putenv(tzSetString) == 0) {
		return 1;
	} else {
		return 0;
	}
}

int
reset_tz()
{
	if (putenv(tz_save) == 0) {
		return 1;
	} else {
		return 0;
	}
}

time_t
set_datetime(struct datetime idt)
{
	int date;
	int time;
	struct tm tm;

	memset(&tm, 0, sizeof(tm));
	tm.tm_isdst = -1;

	if (idt.date == 0) {
		return -1;
	}
	date = idt.date;
	time = idt.time;
	tm.tm_year = date / 10000 - 1900;
	date = date % 10000;
	tm.tm_mon = date / 100 - 1;
	tm.tm_mday = date % 100;
	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec = 0;
	if (time > 9999) {
		tm.tm_sec = time % 100;
		time = time / 100;
	}
	tm.tm_hour = time / 100;
	tm.tm_min = time % 100;

	return mktime(&tm);
}

time_t
set_curr_datetime()
{
	return time((time_t *) 0);
}

time_t
set_yyyymmdd_datetime(char *dt)
{
	struct tm tm;
	int len;
	char datetime[16];

	if ((len = strlen(dt)) != 8 && len != 13 && len != 15) {
		return (time_t)-1; // incorrect date format
	}
	strcpy(datetime, dt);

	memset((char *)&tm, 0, sizeof(tm));
	tm.tm_isdst = -1; // let it decide about Daylight Savings Time

	if (len > 8) {
		if (len == 15) {
			tm.tm_sec = atoi(dt+13);
			datetime[13] = '\0';
		} else {
			tm.tm_sec = 0;
		}
		tm.tm_min = atoi(datetime + 11);
		datetime[11] = '\0';
		tm.tm_hour = atoi(datetime + 9);
		datetime[8] = '\0';
	} else {
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;
	}
	tm.tm_mday = atoi(datetime+6);
	datetime[6] = '\0';
	tm.tm_mon = atoi(datetime+4) - 1;
	datetime[4] = '\0';
	tm.tm_year = atoi(datetime) - 1900;

	return mktime(&tm);
}

struct datetime
get_datetime(time_t time, int get_seconds)
{
	struct tm *tm_p;
	struct datetime dt;

	tm_p = localtime(&time);
	dt.date =
		(tm_p->tm_year + 1900) * 10000 +
		(tm_p->tm_mon + 1) * 100 +
		tm_p->tm_mday;

	dt.time = (tm_p->tm_hour * 100) + tm_p->tm_min;
	if (get_seconds) {
		dt.time = dt.time * 100 + tm_p->tm_sec;
	}
	return dt;
}

time_t
add_months(time_t time, int increment)
{
	struct tm *tm_p;
	tm_p = localtime(&time);
	tm_p->tm_mon += increment;
	tm_p->tm_isdst = -1;
	return mktime(tm_p);
}

time_t
add_years(time_t time, int increment)
{
	return add_months(time, increment * 12);
}

time_t
add_days(time_t time, int increment)
{
	struct tm *tm_p;
	tm_p = localtime(&time);
	tm_p->tm_mday += increment;
	tm_p->tm_isdst = -1;
	return mktime(tm_p);
}

time_t
add_seconds(time_t time, int increment)
{
	struct tm *tm_p;
	tm_p = localtime(&time);
	tm_p->tm_sec += increment;
	tm_p->tm_isdst = -1;
	return mktime(tm_p);
}

time_t
add_minutes(time_t time, int increment)
{
	return add_seconds(time, increment * 60);
}

time_t
add_hours(time_t time, int increment)
{
	return add_seconds(time, increment * 60 * 60);
}

time_t
get_month_start(time_t time)
{
	struct tm *tm_p;
	tm_p = localtime(&time);
	tm_p->tm_mday = 1;
	tm_p->tm_isdst = -1;
	return mktime(tm_p);
}

time_t
get_month_end(time_t time)
{
	struct tm *tm_p;
	tm_p = localtime(&time);
	tm_p->tm_mon += 1; // set to next month
	tm_p->tm_mday = 0; // back up 1 day to last day of this month
	tm_p->tm_isdst = -1; // let it decide about Daylight Savings Time
	return mktime(tm_p);
}

time_t
get_week_start(time_t time)
{
	struct tm *tm_p;
	tm_p = localtime(&time);
	tm_p->tm_mday -= tm_p->tm_wday; // set day to day minus day of week
					// to get first day of week - Sunday
	tm_p->tm_isdst = -1; // let it decide about Daylight Savings Time
	return mktime(tm_p);
}

time_t
get_week_end(time_t time)
{
	struct tm *tm_p;
	tm_p = localtime(&time);
	tm_p->tm_mday -= tm_p->tm_wday; // set day to day minus day of week
					// to get first day of week - Sunday
	tm_p->tm_mday += 6; // set day of week to last day - Saturday
	tm_p->tm_isdst = -1; // let it decide about Daylight Savings Time
	return mktime(tm_p);
}


char *
fmt_dttime(time_t time, char *format, char *output)
{
	char *ip;
	char *op;
	struct tm *tm_p;
	static char Output[1024];

	tm_p = localtime(&time);
	ip = format;
	op = (output != NULL) ? output : Output;

	while (*ip != '\0') {
		if (*ip != '%') {
			*op++ = *ip++;
			continue;
		}
		switch (*++ip) {
		case 'a': // Abbreviated weekday name. E.g. Wed
			op += sprintf(op, "%s", abb_days[tm_p->tm_wday]);
			break;

		case 'A': // Full weekday name. E.g. Wednesday
			op += sprintf(op, "%s", full_days[tm_p->tm_wday]);
			break;

		case 'b': // Abbreviated month name. E.g. Oct
			op += sprintf(op, "%s", abb_months[tm_p->tm_mon]);
			break;

		case 'B': // Full month name. E.g. October
			op += sprintf(op, "%s", full_months[tm_p->tm_mon]);
			break;

		case 'c': // Complete integer date and time. E.g. 20020722 1324
			op += sprintf(op, "%04d%02d%02d %02d%02d",
				tm_p->tm_year + 1900, tm_p->tm_mon + 1, tm_p->tm_mday,
				tm_p->tm_hour, tm_p->tm_min);
			break;

		case 'C': // Two digit Century
			op += sprintf(op, "%02d", (tm_p->tm_year + 1900) / 100);
			break;

		case 'd': // zero filled Day of the month 01-31
			op += sprintf(op, "%02d", tm_p->tm_mday);
			break;

		case 'e': // space filled day of month " 1" - "31"
			op += sprintf(op, "%2d", tm_p->tm_mday);
			break;

		case 'H': // 24 hour 00-23
			op += sprintf(op, "%02d", tm_p->tm_hour);
			break;

		case 'I': // 12 hour 01-12
			op += sprintf(op, "%02d",
				tm_p->tm_hour == 0 ? 12 : tm_p->tm_hour % 12);
			break;

		case 'j': // day of year 001-366
			op += sprintf(op, "%03d", tm_p->tm_yday + 1);
			break;

		case 'm': // month as 2-digit decimal 01-12
			op += sprintf(op, "%02d", tm_p->tm_mon + 1);
			break;

		case 'M': // minute as 2-digit decimal 00-59
			op += sprintf(op, "%02d", tm_p->tm_min);
			break;

		case 'n': // newline character
			op += sprintf(op, "\n");
			break;

		case 'p': // AM or PM
			op += sprintf(op, "%2s", tm_p->tm_hour < 12 ? "AM" : "PM");
			break;

		case 'Q': // date as YYYY-MM-DD
			op += sprintf(op, "%04d-%02d-%02d",
				tm_p->tm_year + 1900, tm_p->tm_mon + 1, tm_p->tm_mday);

		case 'R': // time as HH:MM
			op += sprintf(op, "%02d:%02d", tm_p->tm_hour, tm_p->tm_min);
			break;

		case 'S': // seconds as 2-digit decimal 00-61
			op += sprintf(op, "%02d", tm_p->tm_sec);
			break;

		case 't': // tab character
			op += sprintf(op, "\t");
			break;

		case 'u': // day of week as 1-digit decimal 1-7, Monday-Sunday
			op += sprintf(op, "%1d", tm_p->tm_wday == 0 ? 7 : tm_p->tm_wday);
			break;

		case 'U': // week of the year Sunday-Monday 00-53
		{
			int zeroweek_days;
			int week;

			zeroweek_days = 7 - ((tm_p->tm_yday % 7 + tm_p->tm_wday) % 7);
			week = tm_p->tm_yday < zeroweek_days ? 0 :
				(tm_p->tm_yday + 1 - zeroweek_days) / 7 + 1;
			op += sprintf(op, "%02d", week);
		}
			break;

		case 'V': // week of the year Monday-Sunday, 01-53
		{
			int zeroweek_days;
			int week;

			zeroweek_days = (tm_p->tm_yday % 7 + tm_p->tm_wday) % 7;
			zeroweek_days = zeroweek_days == 0 ? 7 : 8 - zeroweek_days;
			week = (tm_p->tm_yday < zeroweek_days && zeroweek_days < 4) ? 53
				: (tm_p->tm_yday + 1 - zeroweek_days) / 7 + 1;
			op += sprintf(op, "%02d", week);
		}
			break;

		case 'w': // day of week as 1-digit decimal 0-6, Sunday-Saturday
			op += sprintf(op, "%1d", tm_p->tm_wday);
			break;

		case 'W': // week of the year Monday-Sunday, 00-53
		{
			int zeroweek_days;
			int week;

			zeroweek_days = (tm_p->tm_yday % 7 + tm_p->tm_wday) % 7;
			zeroweek_days = zeroweek_days == 0 ? 7 : 8 - zeroweek_days;
			week = tm_p->tm_yday < zeroweek_days ? 0 :
				(tm_p->tm_yday + 1 - zeroweek_days) / 7 + 1;
			op += sprintf(op, "%02d", week);
		}
			break;

		case 'x': // integer date representation YYYYMMDD
			op += sprintf(op, "%04d%02d%02d",
				tm_p->tm_year + 1900, tm_p->tm_mon + 1, tm_p->tm_mday);
			break;

		case 'X': // integer time representation HHMM
			op += sprintf(op, "%02d%02d", tm_p->tm_hour, tm_p->tm_min);
			break;

		case 'y': // Year without century as 2-digit decimal 00-99
			op += sprintf(op, "%02d", tm_p->tm_year % 100);
			break;

		case 'Y': // Year with century as 4-digit decimal 1970-2037
			op += sprintf(op, "%04d", tm_p->tm_year + 1900);
			break;

		case 'Z': // time zone name as 3-character abbreviation
			op += sprintf(op, "%3s", tm_p->tm_isdst == 1 ? tzname[1] : tzname[0]);
			break;

		default:
			break;
		}
		ip++;
	}
	*op = '\0';
	return (output != NULL) ? output : Output;
}

int
get_dow(time_t time)
{
	struct tm tm;

	localtime_r(&time, &tm);
	return tm.tm_wday;
}

int
get_isdst(time_t time)
{
	struct tm tm;

	localtime_r(&time, &tm);
	return tm.tm_isdst;
}
