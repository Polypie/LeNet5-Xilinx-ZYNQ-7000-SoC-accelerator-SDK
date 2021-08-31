#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "nn_utils.h"



int min(int a, int b)
{
    if (a < b)
    {
        return a;
    }
    else
    {
        return b;
    }
}



ReLu_t ReLu(conv_t x)
{
    if (x > 0)
    {
        return x;
    }
    else
    {
        return 0;
    }
}


void init_conv2d_config(conv2d_config* cfg,
    conv_input_param input,
    conv_kernel_param kernel,
    conv_tiling_param tiling)
{
    conv_output_param output;
    output.M = kernel.M;
    output.R = input.Y - kernel.K + 1;
    output.C = input.X - kernel.K + 1;

    cfg->cfg_input = input;
    cfg->cfg_k = kernel;
    cfg->cfg_output = output;
    cfg->cfg_tile = tiling;

    printf("%d %d %d\n", output.R, output.C, output.M);
}



void conv2d(conv2d_config* cfg,
    conv_t* input,
    conv_t* kernel,
    conv_t* bias,
    conv_t* output
)
{
    int mm, rr, cc, z, y, x, m, r, c = 0;


    for (mm = 0; mm < cfg->cfg_output.M; mm += cfg->cfg_tile.Tm)
        for (z = 0; z < cfg->cfg_input.Z; z++)
            for (rr = 0; rr < cfg->cfg_output.R; rr += cfg->cfg_tile.Tr)
                for (cc = 0; cc < cfg->cfg_output.C; cc += cfg->cfg_tile.Tc)
                    for (y = 0; y < cfg->cfg_k.K; y++)
                        for (x = 0; x < cfg->cfg_k.K; x++)
                            /*HW UNROLL START*/
                            for (m = mm; m < min(mm + cfg->cfg_tile.Tm, cfg->cfg_output.M); m++)
                                for (r = rr; r < min(rr + cfg->cfg_tile.Tr, cfg->cfg_output.R); r++)
                                    for (c = cc; c < min(cc + cfg->cfg_tile.Tc, cfg->cfg_output.C); c++)
                                    {
                                        int kernel_index = m +
                                            z * cfg->cfg_k.M +
                                            y * cfg->cfg_k.M * cfg->cfg_input.Z +
                                            x * cfg->cfg_k.M * cfg->cfg_input.Z * cfg->cfg_k.K;
                                        int input_index = z +
                                            (cfg->cfg_k.stride * r + y) * cfg->cfg_input.Z +
                                            (cfg->cfg_k.stride * c + x) * cfg->cfg_input.Z * cfg->cfg_input.Y;
                                        int output_index = m +
                                            r * cfg->cfg_output.M +
                                            c * cfg->cfg_output.M * cfg->cfg_output.R;



                                        output[output_index] += input[input_index] * kernel[kernel_index];
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


conv_t* conv_layer(conv2d_config* cfg,
    conv_t* input,
    conv_t* kernel,
    conv_t* bias)
{
    conv_t* output = calloc(cfg->cfg_output.M * cfg->cfg_output.R * cfg->cfg_output.C, sizeof(conv_t));
    conv2d(cfg, input, kernel, bias, output);
    return output;
}

void init_pool_config(pool_config* cfg,
    pool_input_param input,
    pool_output_param output,
    pool_kernel_param kernel)
{
    cfg->cfg_input = input;
    cfg->cfg_output = output;
    cfg->cfg_k = kernel;
}


void pool(pool_config* cfg,
    pool_t* input,
    pool_t* output)
{
    int m, r, c, x, y;

    for (m = 0; m < cfg->cfg_output.M; m++)
        for (r = 0; r < cfg->cfg_output.R; r++)
            for (c = 0; c < cfg->cfg_output.C; c++)
            {
                int output_index = m +
                    r * cfg->cfg_output.M +
                    c * cfg->cfg_output.M * cfg->cfg_output.R;
                int input_index = m +
                    (cfg->cfg_k.stride * r) * cfg->cfg_input.Z +
                    (cfg->cfg_k.stride * c) * cfg->cfg_input.Z * cfg->cfg_input.Y;

                pool_t temp_max = input[input_index];

                for (y = 0; y < cfg->cfg_k.K; y++)
                    for (x = 0; x < cfg->cfg_k.K; x++)
                    {
                        int input_index_kernel = m +
                            (cfg->cfg_k.stride * r + y) * cfg->cfg_input.Z +
                            (cfg->cfg_k.stride * c + x) * cfg->cfg_input.Z * cfg->cfg_input.Y;
                        if (temp_max < input[input_index_kernel])
                        {
                            temp_max = input[input_index_kernel];
                        }
                    }
                output[output_index] = temp_max;

            }

}

pool_t* pool_layer(pool_config* cfg, pool_t* input)
{
    pool_t* output = calloc(cfg->cfg_output.M * cfg->cfg_output.R * cfg->cfg_output.C, sizeof(pool_t));
    pool(cfg, input, output);
    return output;
}


void init_fully_connected_config(fully_connected_config* cfg,
    int input_size,
    int output_size)
{
    cfg->input_size = input_size;
    cfg->output_size = output_size;
    cfg->filter_size = input_size * output_size;
}

void fully_connected(fully_connected_config* cfg,
    fc_t* input,
    fc_t* kernel,
    fc_t* bias,
    fc_t* output)
{
    for (int in = 0; in < cfg->input_size; in++)
    {
        fc_t data = input[in];
        for (int out = 0; out < cfg->output_size; out++)
        {
            int index = in * cfg->output_size + out;
            fc_t mult = data * kernel[index];
            output[out] += mult;
        }
    }

    for(int i = 0; i < cfg->output_size; i++)
        output[i] = ReLu(output[i] + bias[i]);
}


fc_t* fully_connected_layer(fully_connected_config* cfg,
    fc_t* input,
    fc_t* weights,
    fc_t* bias)
{
    fc_t* output = calloc(cfg->output_size, sizeof(fc_t));
    fully_connected(cfg, input, weights, bias, output);
    return output;
}
