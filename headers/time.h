#ifndef _TIME_H
#define _TIME_H

#include "types.h"

#define startOnVirtualMachine true

typedef int16 Millisecond;

typedef int8 Second;

typedef int8 Minute;

typedef int8 Hour;

typedef int32 AbsoluteTimeToday;

typedef int32 DayWeek;

typedef int8 Day;

typedef int16 YearDay;

typedef int8 Decade;

typedef int8 Month;

typedef uint8 Season;

typedef int8 Year;

typedef int32 Century;

typedef int32 Year4Digits;

typedef bool ProgressFlag;

typedef uint8 RegisterB;

struct Time {
	Millisecond millisecond;

	Second second;

	Minute minute;

	Hour hour;

	Day day;

	AbsoluteTimeToday absoluteTimeToday;

	DayWeek dayWeek;

	YearDay yearDay;

	Decade decade;

	Month month;

	Season season;

	Year year;

	Century century;

	Year4Digits year4Digits;
};

void TimeInit(void);

Day calculateFebruaryNumberDays(struct Time time);

AbsoluteTimeToday calculateAbsoluteTimeToday(struct Time time);

DayWeek calculateDayWeek(struct Time time);

YearDay calculateYearDay(struct Time time);

Decade calculateDecade(struct Time time);

Season calculateSeason(struct Time time);

Year4Digits calculateYear4Digits(struct Time time);

struct Time calculateAll(struct Time time);

struct Time calculateVAll(struct Time time);

struct Time calculateVADAll(struct Time time);

void updateProgressFlag(void);

void updateRegisterB(void);

int8 binaryX8ValueToRTCFormatedIfNecessary(int8 value);

Hour binaryHourToRTCFormatedIfNecessary(Hour hour);

Year binaryYearToRTCFormatedIfNecessary(Year year);

struct Time binaryTimeToRTCFormatedIfNecessary(struct Time time);

int8 binaryX8ValueFromRTCFormatedIfNecessary(int8 value);

Hour binaryHourFromRTCFormatedIfNecessary(Hour hour);

Year binaryYearFromRTCFormatedIfNecessary(Year year);

struct Time binaryTimeFromRTCFormatedIfNecessary(struct Time time);

struct Time newTime(void);

struct Time copyTime(struct Time time);

bool timeEquals(struct Time time, struct Time time2);

bool timeNotEquals(struct Time time, struct Time time2);

bool timeBigThan(struct Time time, struct Time time2);

bool timeLessThan(struct Time time, struct Time time2);

bool timeBigOrEqualThan(struct Time time, struct Time time2);

bool timeLessOrEqualThan(struct Time time, struct Time time2);

struct Time add(struct Time time, struct Time time2);

struct Time subtract(struct Time time, struct Time time2);

void setRTCTime(struct Time time);

struct Time loadRTCTime(void);

#endif