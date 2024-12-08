#include "../headers/pit.h"

#include "../headers/idt.h"

#include "../headers/types.h"

#include "../headers/time.h"

#include "../headers/io.h"

#include "../headers/colors.h"

#include "../headers/std.h"

extern struct Time now;

extern const Day numberDaysOfMonthes[12];

extern const Season seasonsOfMonthes[12];

extern uint16 foregroundColor;

uint32 currentFreq = 0;

int16 millisecondDelta = 1;

bool bPITInitialized = false;

void (*everyMillisecond)() = 0;

void (*everySecond)() = 0;

void (*everyMinute)() = 0;

void (*everyHour)() = 0;

void (*everyDay)() = 0;

void (*everyDecade)() = 0;

void (*everyMonth)() = 0;

void (*everySeason)() = 0;

void (*everyYear)() = 0;

void (*everyCentury)() = 0;

void PITInit(void) {
	if (bPITInitialized) return;

	IDTInit();

	IDTIRQInstallHandler(0, &tick);

	PITSetFreq(soundFreq);

	bPITInitialized = true;
}

void PITSetFreq(uint32 freq) {
	if (currentFreq == freq) return;

	uint32 divisor = 1193180 / freq;

	out8(0x43, 0x36);

	out8(0x40, (uint8)(divisor & 0xff));

	out8(0x40, (uint8)((divisor >> 8) & 0xff));

	millisecondDelta = standardFreq / freq;
}

void PITResetFreq(void) {
	PITSetFreq(soundFreq);
}

void tick(struct Registers* regs) {
	now.millisecond += millisecondDelta;
	
	uint8 eachBit = 0b00000000;

	while (now.millisecond >= 1000) {
		now.millisecond -= 1000;

		now.second++;

		eachBit |= 0b00000001;
	}

	while (now.second >= 60) {
		now.second -= 60;

		now.minute++;

		eachBit |= 0b00000010;
	}

	while (now.minute >= 60) {
		now.minute -= 60;

		now.hour++;

		eachBit |= 0b00000100;
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

		eachBit |= 0b00001000;
	}

	while (now.day > numberDaysOfMonthes[now.month >= 12 ? 0 : now.month]) {
		now.day -= numberDaysOfMonthes[now.month >= 12 ? 0 : now.month];

		if (now.day == 0)
			now.day = 1;

		now.month++;

		now.season = seasonsOfMonthes[now.month >= 12 ? 0 : now.month];

		eachBit |= 0b00010000;
	}

	while (now.month >= 12) {
		now.month = 0;

		now.yearDay = 0;

		now.year++;

		now.year4Digits++;

		eachBit |= 0b00100000;
	}

	while (now.year >= 100) {
		now.year -= 100;

		now.century++;

		eachBit |= 0b01000000;
	}

	if ((eachBit & 0b01000000) && everyCentury)
		everyCentury();

	if ((eachBit & 0b00100000) && everyYear)
		everyYear();

	if ((eachBit & 0b00010000) && everyMonth)
		everyMonth();

	if ((eachBit & 0b00001000) && everyDay)
		everyDay();

	if ((eachBit & 0b00000100) && everyHour)
		everyHour();

	if ((eachBit & 0b00000010) && everyMinute)
		everyMinute();

	if ((eachBit & 0b00000001) && everySecond)
		everySecond();

	if (everyMillisecond)
		everyMillisecond();
}

void sleepMilliseconds(Millisecond delay) {
	struct Time past = copyTime(now);

	struct Time need = newTime();

	need.millisecond = delay;

	need = calculateVAll(need);

	while (timeLessThan(subtract(now, past), need));
}

void sleepSeconds(Second delay) {
	struct Time past = copyTime(now);

	struct Time need = newTime();

	need.second = delay;

	need = calculateVAll(need);

	while (timeLessThan(subtract(now, past), need));
}

void sleepMinutes(Minute delay) {
	struct Time past = copyTime(now);

	struct Time need = newTime();

	need.minute = delay;

	need = calculateVAll(need);

	while (timeLessThan(subtract(now, past), need));
}

void sleepHours(Hour delay) {
	struct Time past = copyTime(now);

	struct Time need = newTime();

	need.hour = delay;

	need = calculateVAll(need);

	while (timeLessThan(subtract(now, past), need));
}

void sleepAbsoluteTimeToday(AbsoluteTimeToday delay) {
	struct Time past = copyTime(now);

	struct Time need = newTime();

	need.millisecond = delay % 1000;

	need.second = (delay / 1000) % 60;

	need.minute = (delay / 60 / 1000) % 60;

	need.hour = (delay / 60 / 60 / 1000) % 24;

	need = calculateVAll(need);

	while (timeLessThan(subtract(now, past), need));
}

void sleepYearDays(YearDay delay) {
	struct Time past = copyTime(now);

	struct Time need = newTime();

	need.yearDay = delay;

	need = calculateVAll(need);

	while (timeLessThan(subtract(now, past), need));
}

void sleepDecades(Decade delay) {
	struct Time past = copyTime(now);

	struct Time need = newTime();

	need.day = delay * 10;

	need = calculateVAll(need);

	while (timeLessThan(subtract(now, past), need));
}

void sleepMonthes(Month delay) {
	struct Time past = copyTime(now);

	struct Time need = newTime();

	need.month = delay;

	need = calculateVAll(need);

	while (timeLessThan(subtract(now, past), need));
}

void sleepSeasons(Season delay) {
	struct Time past = copyTime(now);

	struct Time need = newTime();

	need.season = delay;

	need = calculateVAll(need);

	while (timeLessThan(subtract(now, past), need));
}

void sleepYears(Year delay) {
	struct Time past = copyTime(now);

	struct Time need = newTime();

	need.year = delay;

	need = calculateVAll(need);

	while (timeLessThan(subtract(now, past), need));
}

void sleepCenturies(Century delay) {
	struct Time past = copyTime(now);

	struct Time need = newTime();

	need.century = delay;

	need = calculateVAll(need);

	while (timeLessThan(subtract(now, past), need));
}

void sleepYears4Digits(Year4Digits delay) {
	struct Time past = copyTime(now);

	struct Time need = newTime();

	need.year4Digits = delay;

	while (timeLessThan(subtract(now, past), need));
}

void sleepTime(struct Time delay) {
	struct Time past = copyTime(now);

	while (timeLessThan(subtract(now, past), delay));
}