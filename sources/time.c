#include "../headers/time.h"

#include "../headers/std.h"

#include "../headers/format.h"

#include "../headers/pit.h"

#include "../headers/mem.h"

#include "../headers/io.h"

struct Time now;

bool bTimeInitialized = false;

ProgressFlag progressFlag;

RegisterB registerB;

const Day numberDaysOfMonthes[12] = {
	31, 28, 31, // January, February? ( The question mark means 28 - 29 february day ( leap year ) ), March

	30, 31, 30, // April, May, June

	31, 31, 30, // Jule, August, September

	31, 30, 31 // October, November, December
};

const Season seasonsOfMonthes[12] = {
	0, 0, 1, // January, February, March

	1, 1, 2, // April, May, June

	2, 2, 3, // Jule, August, September

	3, 3, 0 // October, November, December
};

void TimeInit(void) {
	if (bTimeInitialized) return;

	now = loadRTCTime();

	PITInit();

	bTimeInitialized = true;
}

Day calculateFebruaryNumberDays(struct Time time) {
	return time.year4Digits % 4 ? 28 : 29;
}

AbsoluteTimeToday calculateAbsoluteTimeToday(struct Time time) {
	return  (time.hour * 60 * 60 * 1000) +
			(time.minute * 60 * 1000) +
			(time.second * 1000) +
			time.millisecond;
}

DayWeek calculateDayWeek(struct Time time) {
	DayWeek result = ((time.year4Digits - 1) * 365) + (time.year4Digits / 4); // Year * 365.25 = January first

	result--;

	for (Month m = 0; m < time.month; m++)
		result += numberDaysOfMonthes[m];

	result += time.day;

	return result % 7;
}

YearDay calculateYearDay(struct Time time) {
	YearDay result = 0;

	for (Month m = 0; m < time.month; m++)
		result += numberDaysOfMonthes[m];

	result += time.day;

	return result;
}

Decade calculateDecade(struct Time time) {
	return time.day / 10;
}

Season calculateSeason(struct Time time) {
	return seasonsOfMonthes[time.month];
}

Year4Digits calculateYear4Digits(struct Time time) {
	return (((Year4Digits)time.century) * 100) + ((Year4Digits)time.year);
}

struct Time calculateAll(struct Time time) {
	struct Time result = copyTime(time);

	result.absoluteTimeToday =  (result.hour * 60 * 60 * 1000) +
								(result.minute * 60 * 1000) +
								(result.second * 1000) +
								result.millisecond;

	result.yearDay = 0;

	for (Month m = 0; m < result.month; m++)
		result.yearDay += numberDaysOfMonthes[m];

	result.yearDay += result.day;

	result.decade = result.day / 10;

	result.season = seasonsOfMonthes[result.month];

	result.year4Digits = (((Year4Digits)result.century) * 100) + ((Year4Digits)result.year);

	result.dayWeek = ((result.year4Digits - 1) * 365) + (result.year4Digits / 4); // Year * 365.25 = January first

	result.dayWeek--;

	result.dayWeek += result.yearDay;

	result.dayWeek %= 7;

	while (result.dayWeek < 0)
		result.dayWeek += 7;

	return result;
}

struct Time calculateVAll(struct Time time) {
	struct Time result = copyTime(time);

	while (result.millisecond >= 1000) {
		result.millisecond -= 1000;

		result.second++;
	}

	while (result.second >= 60) {
		result.second -= 60;

		result.minute++;
	}

	while (result.minute >= 60) {
		result.minute -= 60;

		result.hour++;
	}

	while (now.hour >= 24) {
		now.hour -= 24;

		now.day++;
	}

	while (result.day > numberDaysOfMonthes[result.month >= 12 ? 0 : result.month]) {
		result.day -= numberDaysOfMonthes[result.month >= 12 ? 0 : result.month];

		if (result.day == 0)
			result.day = 1;

		result.month++;
	}

	while (result.month >= 12) {
		result.month = 0;

		result.yearDay = 0;

		result.year++;
	}

	while (result.year >= 100) {
		result.year -= 100;

		result.century++;
	}

	while (result.millisecond < 0) {
		result.millisecond += 1000;

		result.second--;
	}

	while (result.second < 0) {
		result.second += 60;

		result.minute--;
	}

	while (result.minute < 0) {
		result.minute += 60;

		result.hour--;
	}

	while (result.hour < 0) {
		result.hour += 24;

		result.day--;
	}

	while (result.day < 1) {
		result.day += numberDaysOfMonthes[result.month] - 1;

		result.month--;
	}

	while (result.month < 0) {
		result.month += 12;

		result.year--;
	}

	while (result.year < 0) {
		result.year += 100;

		result.century--;
	}

	result.absoluteTimeToday =  (result.hour * 60 * 60 * 1000) +
								(result.minute * 60 * 1000) +
								(result.second * 1000) +
								result.millisecond;

	result.yearDay = 0;

	for (Month m = 0; m < result.month; m++)
		result.yearDay += numberDaysOfMonthes[m];

	result.yearDay += result.day;

	result.decade = result.day / 10;

	result.season = seasonsOfMonthes[result.month >= 12 ? 0 : result.month];

	result.year4Digits = (((Year4Digits)result.century) * 100) + ((Year4Digits)result.year);

	result.dayWeek = ((result.year4Digits - 1) * 365) + (result.year4Digits / 4); // Year * 365.25 = January first

	result.dayWeek--;

	result.dayWeek += result.yearDay;

	result.dayWeek %= 7;

	while (result.dayWeek < 0)
		result.dayWeek += 7;

	return result;
}

struct Time calculateVADAll(struct Time time) {
	struct Time result = copyTime(time);

	while (result.millisecond >= 1000) {
		result.millisecond -= 1000;

		result.second++;
	}

	while (result.second >= 60) {
		result.second -= 60;

		result.minute++;
	}

	while (result.minute >= 60) {
		result.minute -= 60;

		result.hour++;
	}

	while (now.hour >= 24) {
		now.hour -= 24;

		now.day++;

		now.yearDay++;

		now.dayWeek = ((now.year4Digits - 1) * 365) + (now.year4Digits / 4); // Year * 365.25 = January first

		now.dayWeek--;

		now.dayWeek += now.yearDay;

		now.dayWeek %= 7;

		while (now.dayWeek < 0)
			now.dayWeek += 7;

		now.absoluteTimeToday = 0;
	}

	while (result.day > numberDaysOfMonthes[result.month >= 12 ? 0 : result.month]) {
		result.day -= numberDaysOfMonthes[result.month >= 12 ? 0 : result.month];

		if (result.day == 0)
			result.day = 1;

		result.month++;

		result.season = seasonsOfMonthes[result.month >= 12 ? 0 : result.month];
	}

	while (result.month >= 12) {
		result.month = 0;

		result.yearDay = 0;

		result.year++;

		result.year4Digits++;
	}

	while (result.year >= 100) {
		result.year -= 100;

		result.century++;
	}

	while (result.millisecond < 0) {
		result.millisecond += 1000;

		result.second--;
	}

	while (result.second < 0) {
		result.second += 60;

		result.minute--;
	}

	while (result.minute < 0) {
		result.minute += 60;

		result.hour--;
	}

	while (result.hour < 0) {
		result.hour += 24;

		result.day--;
	}

	while (result.day < 1) {
		result.day += numberDaysOfMonthes[result.month] - 1;

		result.month--;
	}

	while (result.month < 0) {
		result.month += 12;

		result.year--;
	}

	while (result.year < 0) {
		result.year += 100;

		result.century--;
	}

	return result;
}

void updateProgressFlag(void) {
	out8(0x70, 0x0a);

	in8(0x71, &progressFlag);

	progressFlag &= 0b10000000;
}

void updateRegisterB(void) {
	out8(0x70, 0x0b);

	in8(0x71, &registerB);
}

int8 binaryX8ValueToRTCFormatedIfNecessary(int8 value) {
	if (!(registerB & 0x04))
		return toBCDX8(value);

	return value;
}

Hour binaryHourToRTCFormatedIfNecessary(Hour hour) {
	if (!(registerB & 0x04))
		return hourToBCD(hour);

	return hour;
}

Year binaryYearToRTCFormatedIfNecessary(Year year) {
	if (!(registerB & 0x04))
		return toBCDX16(year);

	return year;
}

struct Time binaryTimeToRTCFormatedIfNecessary(struct Time time) {
	if (!(registerB & 0x04)) {
		struct Time result = newTime();

		result.second = toBCDX8(time.second);

		result.minute = toBCDX8(time.minute);

		result.hour = hourToBCD(time.hour);

		result.day = toBCDX8(time.day);

		result.month = toBCDX8(time.month);

		result.year = toBCDX16(time.year);

		return result;
	}

	return time;
}

int8 binaryX8ValueFromRTCFormatedIfNecessary(int8 value) {
	if (!(registerB & 0x04))
		return fromBCDX8(value);

	return value;
}

Hour binaryHourFromRTCFormatedIfNecessary(Hour hour) {
	if (!(registerB & 0x04))
		return hourFromBCD(hour);

	return hour;
}

Year binaryYearFromRTCFormatedIfNecessary(Year year) {
	if (!(registerB & 0x04))
		return fromBCDX16(year);

	return year;
}

struct Time binaryTimeFromRTCFormatedIfNecessary(struct Time time) {
	if (!(registerB & 0x04)) {
		struct Time result = newTime();

		result.second = fromBCDX8(time.second);

		result.minute = fromBCDX8(time.minute);

		result.hour = hourFromBCD(time.hour);

		result.day = fromBCDX8(time.day);

		result.month = fromBCDX8(time.month);

		result.year = fromBCDX16(time.year);

		return result;
	}

	return time;
}

struct Time newTime(void) {
	struct Time result;

	result.millisecond = 0;

	result.second = 0;

	result.minute = 0;

	result.hour = 0;

	result.day = 1;

	result.absoluteTimeToday = 0;

	result.dayWeek = 0;

	result.yearDay = 1;

	result.decade = 0;

	result.month = 0;

	result.season = 0;

	result.year = 0;

	result.century = 0;

	result.year4Digits = 0;

	return result;
}

struct Time copyTime(struct Time time) {
	return time;
}

bool timeEquals(struct Time time, struct Time time2) {
	return  time.absoluteTimeToday == time2.absoluteTimeToday &&
			time.yearDay == time2.yearDay &&
			time.year4Digits == time2.year4Digits;
}

bool timeNotEquals(struct Time time, struct Time time2) {
	return  time.absoluteTimeToday != time2.absoluteTimeToday ||
			time.yearDay != time2.yearDay ||
			time.year4Digits != time2.year4Digits;
}

bool timeBigThan(struct Time time, struct Time time2) {
	if ((time.year4Digits - time2.year4Digits) > 0) return true;

	if ((time.yearDay - time2.yearDay) > 0) return true;

	return (time.absoluteTimeToday - time2.absoluteTimeToday) > 0;
}

bool timeLessThan(struct Time time, struct Time time2) {
	if ((time.year4Digits - time2.year4Digits) < 0) return true;

	if ((time.yearDay - time2.yearDay) < 0) return true;

	return (time.absoluteTimeToday - time2.absoluteTimeToday) < 0;
}

bool timeBigOrEqualThan(struct Time time, struct Time time2) {
	if ((time.year4Digits - time2.year4Digits) >= 0) return true;

	if ((time.yearDay - time2.yearDay) >= 0) return true;

	return (time.absoluteTimeToday - time2.absoluteTimeToday) >= 0;
}

bool timeLessOrEqualThan(struct Time time, struct Time time2) {
	if ((time.year4Digits - time2.year4Digits) <= 0) return true;

	if ((time.yearDay - time2.yearDay) <= 0) return true;

	return (time.absoluteTimeToday - time2.absoluteTimeToday) <= 0;
}

struct Time add(struct Time time, struct Time time2) {
	struct Time result = copyTime(time);

	result.millisecond += time2.millisecond;

	result.second += time2.second;

	result.minute += time2.minute;

	result.hour += time2.hour;

	if (result.day > 1 && time2.day > 1)
		result.day += time2.day;

	result.month += time2.month;

	result.year4Digits += time2.year4Digits;

	return calculateVAll(result);
}

struct Time subtract(struct Time time, struct Time time2) {
	struct Time result = copyTime(time);

	result.millisecond -= time2.millisecond;

	result.second -= time2.second;

	result.minute -= time2.minute;

	result.hour -= time2.hour;

	if (result.day > 1 && time2.day > 1)
		result.day -= time2.day;

	if (result.day == 0)
		result.day = 1;

	result.month -= time2.month;

	result.year4Digits -= time2.year4Digits;

	return calculateVAll(result);
}

void setRTCTime(struct Time time) {
	if (startOnVirtualMachine) time.hour -= 3;

	time.month++;

	updateRegisterB();

	time = binaryTimeToRTCFormatedIfNecessary(time);

	out8(0x70, 0x00);

	out8(0x71, time.second);


	out8(0x70, 0x02);

	out8(0x71, time.minute);


	out8(0x70, 0x04);

	out8(0x71, time.hour);


	out8(0x70, 0x07);

	out8(0x71, time.day);


	out8(0x70, 0x08);

	out8(0x71, time.month);


	out8(0x70, 0x09);

	out8(0x71, time.year);
}

struct Time loadRTCTime(void) {
	struct Time result = newTime();

	while (progressFlag) updateProgressFlag();

	uint8 tempSecond = 0;

	out8(0x70, 0x00);

	in8(0x71, &tempSecond);

	result.second = tempSecond;


	uint8 tempMinute = 0;

	out8(0x70, 0x02);

	in8(0x71, &tempMinute);

	result.minute = tempMinute;


	uint8 tempHour = 0;

	out8(0x70, 0x04);

	in8(0x71, &tempHour);

	result.hour = tempHour;

	if (startOnVirtualMachine) result.hour += 3;


	uint8 tempDay = 0;

	out8(0x70, 0x07);

	in8(0x71, &tempDay);

	result.day = tempDay;


	uint8 tempMonth = 0;

	out8(0x70, 0x08);

	in8(0x71, &tempMonth);

	result.month = tempMonth;


	uint8 tempYear = 0;

	out8(0x70, 0x09);

	in8(0x71, &tempYear);

	result.year = (int8)tempYear;

	updateRegisterB();


	result = binaryTimeFromRTCFormatedIfNecessary(result);

	result.month--;
	
	result.century = 20;

	return calculateAll(result);
}