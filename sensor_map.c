/*++

Copyright (c) 2016 Wisky.  All Rights Reserved.


Revision History:

2016-03-08 Created by Fibo Lu

--*/
//#include "r_cg_macrodriver.h"


const unsigned short conVoltageToTemperatureMap[81] =
{
	2955, //	{ -20
	2937, //	{ -19
	2919, //	{ -18
	2900, //	{ -17
	2880, //	{ -16
	2860, //	{ -15
	2839, //	{ -14
	2817, //	{ -13
	2795, //	{ -12
	2772, //	{ -11
	2748, //	{ -10
	2724, //	{ -9
	2699, //	{ -8
	2674, //	{ -7
	2648, //	{ -6
	2621, //	{ -5
	2594, //	{ -4
	2566, //	{ -3
	2537, //	{ -2
	2509, //	{ -1
	2479, //	{ 0,
	2449, //	{ 1,
	2419, //	{ 2,
	2388, //	{ 3,
	2356, //	{ 4,
	2324, //	{ 5,
	2292, //	{ 6,
	2260, //	{ 7,
	2227, //	{ 8,
	2194, //	{ 9,
	2160, //	{ 10
	2127, //	{ 11
	2093, //	{ 12
	2059, //	{ 13
	2025, //	{ 14
	1991, //	{ 15
	1956, //	{ 16
	1922, //	{ 17
	1888, //	{ 18
	1854, //	{ 19
	1819, //	{ 20
	1785, //	{ 21
	1751, //	{ 22
	1717, //	{ 23
	1684, //	{ 24
	1650, //	{ 25
	1617, //	{ 26
	1584, //	{ 27
	1551, //	{ 28
	1518, //	{ 29
	1486, //	{ 30
	1454, //	{ 31
	1423, //	{ 32
	1392, //	{ 33
	1361, //	{ 34
	1331, //	{ 35
	1301, //	{ 36
	1271, //	{ 37
	1242, //	{ 38
	1214, //	{ 39
	1186, //	{ 40
	1158, //	{ 41
	1131, //	{ 42
	1104, //	{ 43
	1078, //	{ 44
	1052, //	{ 45
	1027, //	{ 46
	1002, //	{ 47
	978, //	{ 48
	954, //	{ 49
	931, //	{ 50
	908, //	{ 51
	886, //	{ 52
	864, //	{ 53
	843, //	{ 54
	822, //	{ 55
	801, //	{ 56
	781, //	{ 57
	762, //	{ 58
	743, //	{ 59
	724, //	{ 60
	};


char getTemperatureFromADCValue(unsigned short adcValue)
{
	unsigned short voltageInmV;
	char n, i;

	voltageInmV = (unsigned short)((unsigned long)adcValue * 3300 / 1023);

	for (n = 0; n <= 80; n += 10)
	{
		if (voltageInmV >= conVoltageToTemperatureMap[n])
			break;
	}

	if (n <= 0)
		return -20;
	else if (n > 80)
		return 60;

	for (i = n - 10 + 1; i <= n; i++)
	{
		if (voltageInmV >= conVoltageToTemperatureMap[i])
			break;
	}

	return (i - 20);
}

const unsigned short conVoltageToHumidity[41 * 7] =
{
	/*5 14*/
	/*90*/	260,	265,	269,	272,	276,	278,	281,	284,	286,	289,
	/*80*/	206,	212,	218,	224,	229,	234,	239,	244,	248,	253,
	/*70*/	137,	144,	150,	157,	162,	168,	174,	180,	186,	192,
	/*60*/	72,		76,		81,		86,		90,		94,		100,	106,	111,	117,
	/*50*/	28,		30,		32,		34,		36,		38,		41,		45,		48,		52,
	/*40*/	8,		9,		9,		10,		11,		11,		12,		13,		14,		15,
	/*30*/	1,		1,		2,		2,		2,		2,		2,		3,		3,		3,
	/*15 24*/
	/*90*/	291,	293,	294,	296,	297,	299,	300,	301,	302,	303,
	/*80*/	257,	260,	263,	266,	269,	271,	274,	276,	279,	281,
	/*70*/	198,	203,	208,	213,	218,	223,	227,	232,	236,	240,
	/*60*/	123,	129,	135,	141,	148,	155,	160,	166,	171,	177,
	/*50*/	55,		59,		63,		66,		71,		75,		79,		84,		89,		94,
	/*40*/	17,		18,		20,		21,		23,		25,		27,		30,		32,		35,
	/*30*/	3,		4,		4,		5,		5,		6,		6,		7,		8,		8,
	/*25 34*/
	/*90*/	304,	305,	306,	307,	307,	308,	309,	309,	310,	311,
	/*80*/	283,	285,	287,	289,	290,	292,	293,	295,	296,	298,
	/*70*/	244,	248,	252,	255,	258,	262,	265,	268,	271,	274,
	/*60*/	183,	189,	194,	200,	205,	211,	216,	220,	225,	230,
	/*50*/	100,	105,	111,	117,	123,	129,	135,	141,	147,	153,
	/*40*/	39,		42,		45,		49,		53,		58,		61,		66,		71,		76,
	/*30*/	9,		10,		11,		12,		13,		15,		16,		18,		19,		21,
	/*35 45*/
	/*90*/	311,	312,	312,	313,	313,	314,	314,	314,	315,	315,	315,
	/*80*/	299,	300,	301,	302,	303,	304,	305,	305,	306,	307,	307,
	/*70*/	277,	279,	281,	283,	285,	287,	288,	290,	291,	293,	294,
	/*60*/	235,	239,	242,	246,	249,	253,	256,	258,	261,	264,	267,
	/*50*/	160,	166,	171,	177,	183,	189,	193,	198,	203,	208,	213,
	/*40*/	82,		86,		91,		96,		101,	107,	111,	116,	121,	126,	132,
	/*30*/	23,		25,		27,		29,		31,		34,		37,		39,		42,		45,		49,
};


unsigned char getHumidityFromADCValue(char temperature, unsigned short adcValue)
{ 
	unsigned short voltageIn10mV;
	unsigned short humidity = 50;
	unsigned short step, t, i;
	unsigned short hPrev, hNext;
	unsigned short temperatureU;

	voltageIn10mV = (unsigned short)((unsigned long)adcValue * 330 / 1023);
	
	if (temperature < 5 || temperature > 45)
		return 50;

	temperatureU = (unsigned short)temperature;
	if (temperatureU < 35)
	{
		step = 10;
		t = 10 * 7 * ((temperatureU - 5) / 10);
		t += ((temperatureU - 5) % 10);
	}
	else
	{
		step = 11;
		t = 10 * 7 * 3;
		t += (temperatureU - 35);
	}
	

	for (i = 0; i < 7; i++)
	{
		if (voltageIn10mV > conVoltageToHumidity[t + step * i])
			break;
	}

	if (i == 0)
	{
		humidity = 90;
	}
	else if (i == 7)
	{
		humidity = 30;
	}
	else
	{
		hPrev = conVoltageToHumidity[t + step * i - step];
		hNext = conVoltageToHumidity[t + step * i];
		if (hPrev <= hNext)
			humidity = 90 - i * 10 + 5;
		else
			humidity = 90 - i * 10 + (voltageIn10mV - hNext) * 10 / (hPrev - hNext);
	}

	return (unsigned char)humidity;
}


unsigned short getVoltageIn10mVFromADCValue(unsigned short adcValue)
{
	unsigned short voltageIn10mV;

	voltageIn10mV = (unsigned short)((unsigned long)adcValue * 330 / 1023);
	return voltageIn10mV;
}


