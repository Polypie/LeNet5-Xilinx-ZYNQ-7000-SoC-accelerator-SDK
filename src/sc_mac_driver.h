/*
 * sc_mac_driver.h
 *
 *  Created on: 20 Nov 2020
 *      Author: Jacob
 */

#ifndef SRC_SC_MAC_DRIVER_H_
#define SRC_SC_MAC_DRIVER_H_

#include "nn_utils.h"


typedef int s_mag;
typedef int comp;

typedef struct
{
	int check_05;
	int check_025;
	int check_0125;

}sc_mac_types;

typedef struct
{
	int input_bit_width;
	int acc_bit_width;
	int output_bit_width;
}
sc_mac_bitwidths;

typedef struct
{
	int Tm;
	int Tr;
	int Tc;
}sc_mac_tiling;

typedef struct
{
	int input;
	int output;
}sc_mac_bitmask;

typedef struct
{
	int len_bias_reg;
	int len_weights_reg;
	int len_inputs_reg;
	int len_outputs_reg;

}sc_mac_register;



typedef struct
{
	int bias;
	int weights;
	int inputs;
	int outputs;

}sc_mac_array_len;

typedef struct
{
	int bias_offset;
	int weights_offset;
	int inputs_offset;
	int outputs_offset;

}sc_mac_register_offset;



typedef struct
{
	int base_address;
	int denominator;
	int cycles;
	sc_mac_types types;
	sc_mac_bitwidths bitwidths;
	sc_mac_bitmask bitmask;
	sc_mac_tiling tiling;
	sc_mac_register reg;
	sc_mac_array_len len;
	sc_mac_register_offset offset;
}
sc_mac_settings;


void test_sc_mac_get_config();

void init_sc_mac_settings(sc_mac_settings * cfg, int base_address);

void set_tile_from_mac(sc_mac_settings* cfg, conv2d_config *conv_cfg);
//void init_sc_mac_lenet5(sc_mac_settings* cfg, lenet5_config *lenet5_cfg);

int get_control_reg(int base_address);
void set_control_reg(int base_address, int value);

void set_bias_regs(sc_mac_settings *cfg, comp *array);
void set_weight_regs(sc_mac_settings *cfg, s_mag *array);
void set_input_regs(sc_mac_settings *cfg, s_mag *array);
void get_output_regs(sc_mac_settings *cfg, comp *array);





void conv2d_mac_acc(conv2d_config* cfg,
					sc_mac_settings *sc_cfg,
					conv_t *input,
					conv_t *kernel,
					conv_t *bias,
					conv_t *output);

conv_t* conv_layer_mac_acc(conv2d_config* cfg,
						   sc_mac_settings *sc_cfg,
                   	   	   conv_t* input,
						   conv_t* kernel,
						   conv_t* bias);

#endif /* SRC_SC_MAC_DRIVER_H_ */
