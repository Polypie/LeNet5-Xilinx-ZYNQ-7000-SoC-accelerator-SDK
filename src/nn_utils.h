/*
 * lenet5.h
 *
 *  Created on: 15 Nov 2020
 *      Author: Jacob
 */

#ifndef SRC_NN_UTILS_H_
#define SRC_NN_UTILS_H_


typedef float conv_t;
typedef float ReLu_t;

typedef struct
{
    int M;
    int R;
    int C;
} conv_output_param;

typedef struct
{
    int Z;
    int Y;
    int X;
} conv_input_param;


typedef struct
{
    int K;
    int M;
    int stride;
} conv_kernel_param;


typedef struct
{
    int Tm;
    int Tr;
    int Tc;
} conv_tiling_param;


typedef struct
{
    conv_output_param cfg_output;
    conv_input_param  cfg_input;
    conv_kernel_param cfg_k;
    conv_tiling_param cfg_tile;
}conv2d_config;



void init_conv2d_config(conv2d_config* cfg,
    conv_input_param input,
    conv_kernel_param kernel,
    conv_tiling_param tiling);

void conv2d(conv2d_config* cfg,
    conv_t* input,
    conv_t* kernel,
    conv_t* bias,
    conv_t* output
);

ReLu_t ReLu(conv_t x);

conv_t* conv_layer(conv2d_config* cfg,
                   conv_t* input,
                   conv_t* kernel,
                   conv_t* bias);



typedef float pool_t;

typedef struct
{
    int Z;
    int Y;
    int X;
}pool_input_param;

typedef struct
{
    int M;
    int R;
    int C;
} pool_output_param;


typedef struct
{
    int K;
    int M;
    int stride;
} pool_kernel_param;


typedef struct
{
    pool_input_param cfg_input;
    pool_output_param cfg_output;
    pool_kernel_param cfg_k;
} pool_config;

int min(int a, int b);
void init_pool_config(pool_config* cfg,
    pool_input_param input,
    pool_output_param output,
    pool_kernel_param kernel);

void pool(pool_config* cfg,
    pool_t* input,
    pool_t* output);

pool_t* pool_layer(pool_config* cfg, pool_t* input);

typedef float fc_t;

typedef struct
{
    int input_size;
    int output_size;
    int filter_size;
} fully_connected_config;


void init_fully_connected_config(fully_connected_config* cfg,
    int input_size,
    int output_size);

void fully_connected(fully_connected_config* cfg,
    fc_t* input,
    fc_t* kernel,
    fc_t* bias,
    fc_t* output);


fc_t* fully_connected_layer(fully_connected_config* cfg,
    fc_t* input,
    fc_t* weights,
    fc_t* bias);


#endif /* SRC_NN_UTILS_H_ */
