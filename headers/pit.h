#ifndef _PIT_H
#define _PIT_H

#include "idt.h"

#include "time.h"

#define standardFreq 1000 // one millisecond

#define soundFreq 100 // 100 Hz choosed because this Hz use by speaker ( PC speaker ) for sounds

void PITInit(void);

void PITSetFreq(uint32 freq);

void PITResetFreq(void);

void tick(struct Registers* regs);

void sleepMilliseconds(Millisecond delay);

void sleepSeconds(Second delay);

void sleepMinutes(Minute delay);

void sleepHours(Hour delay);

void sleepDays(Day delay);

void sleepAbsoluteTimeToday(AbsoluteTimeToday delay);

void sleepYearDays(YearDay delay);

void sleepDecades(Decade delay);

void sleepMonthes(Month delay);

void sleepSeasons(Season delay);

void sleepYears(Year delay);

void sleepCenturies(Century delay);

void sleepYears4Digits(Year4Digits delay);

void sleepTime(struct Time delay);

#endif