/*
 * number_systems_ca.h
 *
 *  Created on: 21 Nov 2020
 *      Author: Jacob
 */


#ifndef SRC_NUMBER_SYSTEMS_CA_H_
#define SRC_NUMBER_SYSTEMS_CA_H_

#include <math.h>
#define SIGNEX(v, sb) ((v) | (((v) & (1 << (sb))) ? ~((1 << (sb))-1) : 0))


int float_2_s_mag(float a, int denominator, int bit_width)
{
	int int_a = denominator * a;//2COMP

	if (int_a >= denominator)
	{
		int_a = denominator - 1;
	}

	if (int_a <= (-denominator))
	{
		int_a = -denominator + 1;
	}

	if (int_a < 0)//NEGATIVE
	{
		int_a = ((~int_a)+1) | (1 << bit_width);
		int_a = SIGNEX(int_a, bit_width);
	}
	return int_a;
}

int float_2_int(float a, int denominator)
{
	int int_a = denominator * a;
	if (int_a >= denominator)
	{
		int_a = denominator - 1;
	}

	if (int_a <= -denominator)
	{
		int_a = -denominator + 1;
	}
	//printf("int_a %d \n", int_a);

	return int_a;
}


float int_2_float(int a, int denominator)
{
	float float_a = (float)a / (float)denominator;
	return float_a;
}

int ipow(int base, int exp)
{
    int result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

int is_bit_enabled(int value, int bit)
{
	int is_enabled = 0;
	if(value & (1 << bit))
	{
		is_enabled = 1;
	}
	return is_enabled;
}


int int_ceil(int a, int b)
{
	return a/b + (a % b != 0);
}

int int_min(int a, int b)
{
	if(a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

#endif
