/*
 * energy_time_sampling.h
 *
 *  Created on: 30 Nov 2020
 *      Author: Jacob
 */

#ifndef SRC_ENERGY_TIME_SAMPLING_H_
#define SRC_ENERGY_TIME_SAMPLING_H_


int init_adc();
int init_time();
void sample_adc();

void sample_time();

void print_stats(int cycles);


#endif /* SRC_ENERGY_TIME_SAMPLING_H_ */
