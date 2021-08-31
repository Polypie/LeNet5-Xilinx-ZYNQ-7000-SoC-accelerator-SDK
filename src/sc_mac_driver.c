/*
 *  sc_mac_driver.c
 *
 *  Created on: 21 Nov 2020
 *      Author: Jacob
 */

#include <stdio.h>
#include <stdlib.h>
#include "sc_mac_driver.h"
#include "number_systems_ca.h"
#include "xil_io.h"
#include "xparameters.h"

#define CONTROL_REG 0
#define TILE_REG 4
#define MULT_ACC_REG 8
#define TIMER_REG 12



void init_sc_mac_settings(sc_mac_settings * cfg, int base_address)
{
	cfg->base_address = base_address;
	int temp;


	temp = Xil_In32(cfg->base_address+TILE_REG);

	cfg->tiling.Tm = (temp & 0x000000FF);
	cfg->tiling.Tr = (temp & 0x0000FF00)>>8;
	cfg->tiling.Tc = (temp & 0x00FF0000)>>16;

	temp = Xil_In32(cfg->base_address+MULT_ACC_REG);

	cfg->types.check_05 = temp & 1;
	cfg->types.check_025 = (temp & 2) >> 1;
	cfg->types.check_0125 = (temp & 4) >> 2;
	cfg->bitwidths.input_bit_width = (temp >> 3) & 0x000000FF;
	cfg->bitmask.input = ipow(2, cfg->bitwidths.input_bit_width+1)-1;
	cfg->denominator = ipow(2, cfg->bitwidths.input_bit_width);

	cfg->bitwidths.acc_bit_width = (temp >> 11 ) & 0x000000FF;
	cfg->bitwidths.output_bit_width = (temp >> 19 ) & 0x000000FF;
	cfg->bitmask.output= ipow(2, cfg->bitwidths.output_bit_width+1)-1;

	cfg->offset.weights_offset = 16;
	cfg->offset.bias_offset = cfg->offset.weights_offset+4*int_ceil(cfg->tiling.Tm, 3);

	cfg->offset.outputs_offset = 16;
	cfg->offset.inputs_offset = cfg->offset.bias_offset+4*int_ceil(cfg->tiling.Tm, 3);



	cfg->reg.len_bias_reg = int_ceil(cfg->tiling.Tm, 3);
	cfg->reg.len_weights_reg  = int_ceil(cfg->tiling.Tm, 3);
	cfg->reg.len_inputs_reg = int_ceil(cfg->tiling.Tc*cfg->tiling.Tr, 3);
	cfg->reg.len_outputs_reg = cfg->tiling.Tc*cfg->tiling.Tm*cfg->tiling.Tr;

	cfg->len.bias = cfg->tiling.Tm;
	cfg->len.inputs = cfg->tiling.Tr * cfg->tiling.Tc;
	cfg->len.weights = cfg->tiling.Tm;
	cfg->len.outputs = cfg->tiling.Tc*cfg->tiling.Tm*cfg->tiling.Tr;


}


int get_control_reg(int base_address)
{
	return Xil_In32(base_address);
}


void set_control_reg(int base_address, int value)
{
	return Xil_Out32(base_address, value);
}


void set_bias_regs(sc_mac_settings *cfg, comp *array)
{
	for(int j = 0; j < cfg->len.bias; j++ )
		{
			int temp = 0;
			int reg_sec_index = 0;
			for(int i = j*3; i < int_min(cfg->len.bias, (1+j)*3); i++)
			{
				temp = temp | (  (array[i] & cfg->bitmask.input ) << (reg_sec_index * (cfg->bitwidths.input_bit_width+1)));
				reg_sec_index++;

			}
			Xil_Out32(cfg->base_address+cfg->offset.bias_offset+(4*j), temp);
		}
}

void set_weight_regs(sc_mac_settings *cfg, s_mag *array)
{
	for(int j = 0; j < cfg->len.bias; j++ )
	{
		int temp = 0;
		int reg_sec_index = 0;
		for(int i = j*3; i < int_min(cfg->len.weights, (1+j)*3); i++)
		{
			temp = temp | (  (array[i] & cfg->bitmask.input ) << (reg_sec_index * (cfg->bitwidths.input_bit_width+1)));
			reg_sec_index++;
		}
		Xil_Out32(cfg->base_address+cfg->offset.weights_offset+(4*j), temp);
	}
}

void set_input_regs(sc_mac_settings *cfg, s_mag *array)
{
	for(int j = 0; j < cfg->len.inputs; j++ )
	{
			int temp = 0;
			int reg_sec_index = 0;
			for(int i = j*3; i < int_min(cfg->len.inputs, (1+j)*3); i++)
			{
				temp = temp | (  (array[i] & cfg->bitmask.input ) << (reg_sec_index * (cfg->bitwidths.input_bit_width+1)));
				reg_sec_index++;
			}
			Xil_Out32(cfg->base_address+cfg->offset.inputs_offset+(4*j), temp);
	}
}

void get_output_regs(sc_mac_settings *cfg, comp *array)
{
	for(int i = 0; i < cfg->len.outputs; i++)
	{
		array[i] = SIGNEX( Xil_In32(cfg->base_address+cfg->offset.outputs_offset+(4*i)), cfg->bitwidths.output_bit_width);
	}
}



void test_sc_mac_get_config()
{
	sc_mac_settings cfg;
	init_sc_mac_settings(&cfg, XPAR_SC_CONV_LAYER_MAC_0_S00_AXI_BASEADDR);


	printf("*********************TILE REG**************************\n");
	printf("Tm %d Tr %d Tc %d \n",  cfg.tiling.Tm,
									cfg.tiling.Tr,
									cfg.tiling.Tc);

	printf("*********************MULT CONF REG***********************\n");
	printf("half_check %d quarter_check %d eight_check %d \n",  cfg.types.check_05,
																cfg.types.check_025,
																cfg.types.check_0125);
	printf("*********************ACC CONF REG***********************\n");
	printf("bit_width %d acc_width %d output_width %d \n", cfg.bitwidths.input_bit_width,
														   cfg.bitwidths.acc_bit_width,
														   cfg.bitwidths.output_bit_width);

	printf("**********************REG OFFSETS*************************\n");
	printf("weight_reg_offset %d bias_reg_offset %d input_reg_offset %d output_reg_offset %d\n", cfg.offset.weights_offset,
															   	   	   	   	   	   	   	   	   	 cfg.offset.bias_offset,
																							   	 cfg.offset.inputs_offset,
																								 cfg.offset.outputs_offset);

	printf("******************INPUT AND OUTPUT LEN*********************\n");
	printf("bias_len %d weight_len %d input_len %d output_len %d\n", cfg.len.bias, cfg.len.weights, cfg.len.inputs, cfg.len.outputs);

	printf("******************BITMASKS *******************************\n");
	printf("bit_width_mask %d output_width_mask %d\n", cfg.bitmask.input, cfg.bitmask.output);
	printf("******************DENOMINATOR *******************************\n");
	printf("denominator %d\n", cfg.denominator);


}


void set_tile_from_mac(sc_mac_settings* cfg, conv2d_config *conv_cfg)
{
	conv_cfg->cfg_tile.Tm = cfg->tiling.Tm;
	conv_cfg->cfg_tile.Tr = cfg->tiling.Tr;
	conv_cfg->cfg_tile.Tc = cfg->tiling.Tc;
}

void print_conv_layer(conv_t *input)
{
	for(int i = 0; i < 24*24*6; i++)
	{
		printf("%f ",input[i] );
		if(i%24 == 23)
			printf("\n");

		if( i%(24*24) == (24*24)-1)
			printf("\n");
	}
}

void print_conv_tile(int *input)
{
	for(int i = 0; i < 6*6; i++)
	{
		printf("%f ",  int_2_float(input[i], 256));
		if(i%6 == 5)
			printf("\n");

		if( i%36 == 35)
			printf("\n");
	}

}


void print_conv_input(int *input)
{
	for(int i = 0; i < 6*6; i++)
	{
		printf("%f ", int_2_float(input[i], 256) );
		if(i%6 == 5)
			printf("\n");
	}
}

comp bias_tile[6]  ;
s_mag weight_tile[6]  ;
s_mag input_tile[36] ;
comp  output_tile[6*6*6] ;

void conv2d_mac_acc(conv2d_config* cfg,
					sc_mac_settings *sc_cfg,
					conv_t *input,
					conv_t *kernel,
					conv_t *bias,
					conv_t *output)

{
	int mm, rr, cc, z, y, x, m, r, c = 0;
	int cycles = 0;

	    for (mm = 0; mm < cfg->cfg_output.M; mm += cfg->cfg_tile.Tm)
	        for (z = 0; z < cfg->cfg_input.Z; z++)
	            for (rr = 0; rr < cfg->cfg_output.R; rr += cfg->cfg_tile.Tr)
	                for (cc = 0; cc < cfg->cfg_output.C; cc += cfg->cfg_tile.Tc)
	                {
	                	int count = 0;
	                	for (y = 0; y < cfg->cfg_k.K; y++)
	                    for (x = 0; x < cfg->cfg_k.K; x++)
	                    {
	                    	for (m = mm; m < min(mm + cfg->cfg_tile.Tm, cfg->cfg_output.M); m++)
	                        {
	                            if(x == 0 && y == 0)
	                            {
	                            	bias_tile[m-mm] = float_2_int(bias[m], sc_cfg->denominator);
	                            }
	                            int kernel_index = m +
	                            	               z * cfg->cfg_k.M +
	                            	               y * cfg->cfg_k.M * cfg->cfg_input.Z +
	                            	               x * cfg->cfg_k.M * cfg->cfg_input.Z * cfg->cfg_k.K;
	                            weight_tile[m-mm] = float_2_s_mag(kernel[kernel_index], sc_cfg->denominator, sc_cfg->bitwidths.input_bit_width);
	                        }

	                        for (r = rr; r < min(rr + cfg->cfg_tile.Tr, cfg->cfg_output.R); r++)
	                        {
	                           	for (c = cc; c < min(cc + cfg->cfg_tile.Tc, cfg->cfg_output.C); c++)
	                            {
	                            	int input_index = z +
	                                    			 (cfg->cfg_k.stride * r + y) * cfg->cfg_input.Z +
	                                    		     (cfg->cfg_k.stride * c + x) * cfg->cfg_input.Z * cfg->cfg_input.Y;
	                            	input_tile[(c-cc) + (r-rr) * cfg->cfg_tile.Tc] = float_2_s_mag(input[input_index],  sc_cfg->denominator, sc_cfg->bitwidths.input_bit_width);

	                            }
	                        }

	                        if(x == 0 && y == 0)
	                        	set_bias_regs(sc_cfg, bias_tile);
	                        set_weight_regs(sc_cfg, weight_tile);
	                        set_input_regs(sc_cfg, input_tile);

	                        if(x == 0 && y == 0)
	                        {
	                        	set_control_reg(sc_cfg->base_address, 5);//SET CONV_START AND FRAME_READY
	                        	set_control_reg(sc_cfg->base_address, 0);//CLEAR CONV_START AND FRAME_READY
	                        }
	                        else
	                        {
	                        	set_control_reg(sc_cfg->base_address, 4);//SET FRAME_READY
	                        	set_control_reg(sc_cfg->base_address, 0);//CLEAR FRAME_READY
	                        }



	                        int frame_done = 1;
	                        while(frame_done == 1)
	                        {
	                        	int frame = (get_control_reg(sc_cfg->base_address)&8);

	                        	if(frame == 8)
	                        	{
	                        		count++;
	                        		frame_done = 0;
	                        	}

	                        }//WAIT UNTIL FRAME IS READY



	                    }
	                	int conv_done = 1;
	                	while(conv_done == 1)
	                	{
	                		int done = (get_control_reg(sc_cfg->base_address)&2);

	                		if(done == 2)
	                			conv_done = 0;
	                	}


	                	cycles = Xil_In32(sc_cfg->base_address+TIMER_REG);
	                	sc_cfg->cycles = cycles;

	                	get_output_regs(sc_cfg, output_tile);

	                	for (m = mm; m < min(mm + cfg->cfg_tile.Tm, cfg->cfg_output.M); m++)
	                	for (r = rr; r < min(rr + cfg->cfg_tile.Tr, cfg->cfg_output.R); r++)
	                	for (c = cc; c < min(cc + cfg->cfg_tile.Tc, cfg->cfg_output.C); c++)
	                	{
	                		int output_index = m +
	                			               r * cfg->cfg_output.M +
	                			               c * cfg->cfg_output.M * cfg->cfg_output.R;

	                		int output_tile_index = (c-cc) +
	                								(r-rr) * cfg->cfg_tile.Tc +
													(m-mm) * cfg->cfg_tile.Tr *  cfg->cfg_tile.Tc ;
	                		output[output_index] = int_2_float(output_tile[output_tile_index], sc_cfg->denominator);

	                	}


	                }



	    for (m = 0; m < cfg->cfg_output.M; m++)
	        for (r = 0; r < cfg->cfg_output.R; r++)
	            for (c = 0; c < cfg->cfg_output.C; c++)
	            {
	                int output_index = m +
	                    r * cfg->cfg_output.M +
	                    c * cfg->cfg_output.M * cfg->cfg_output.R;
	                output[output_index] = ReLu(output[output_index] + bias[m]);
	            }


}

conv_t* conv_layer_mac_acc(conv2d_config* cfg,
						   sc_mac_settings *sc_cfg,
                   	   	   conv_t* input,
						   conv_t* kernel,
						   conv_t* bias)

{
	conv_t* output = calloc(cfg->cfg_output.M * cfg->cfg_output.R * cfg->cfg_output.C, sizeof(conv_t));
	conv2d_mac_acc(cfg, sc_cfg, input, kernel, bias, output);
	return output;
}
