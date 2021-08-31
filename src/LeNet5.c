#include "LeNet5.h"
#include "nn_utils.h"
#include "LeNet5_weights_bias.h"
//#include "sc_mac_driver.h"
#include <stdlib.h>
#include "xparameters.h"

void lenet5_config_init(lenet5_config* cfg)
{
    cfg->layer1.cfg_input.X = 28;
    cfg->layer1.cfg_input.Y = 28;
    cfg->layer1.cfg_input.Z = 1;

    cfg->layer1.cfg_tile.Tm = 1;
    cfg->layer1.cfg_tile.Tr = 1;
    cfg->layer1.cfg_tile.Tc = 1;

    cfg->layer1.cfg_k.K = 5;
    cfg->layer1.cfg_k.M = 6;
    cfg->layer1.cfg_k.stride = 1;

    init_conv2d_config(&cfg->layer1, cfg->layer1.cfg_input, cfg->layer1.cfg_k, cfg->layer1.cfg_tile);


    cfg->layer2.cfg_input.X = 24;
    cfg->layer2.cfg_input.Y = 24;
    cfg->layer2.cfg_input.Z = 6;

    cfg->layer2.cfg_k.K = 2;
    cfg->layer2.cfg_k.M = 6;
    cfg->layer2.cfg_k.stride = 2;

    cfg->layer2.cfg_output.C = 12;
    cfg->layer2.cfg_output.R = 12;
    cfg->layer2.cfg_output.M = 6;

    init_pool_config(&cfg->layer2, cfg->layer2.cfg_input, cfg->layer2.cfg_output, cfg->layer2.cfg_k);

    cfg->layer3.cfg_input.X = 12;
    cfg->layer3.cfg_input.Y = 12;
    cfg->layer3.cfg_input.Z = 6;

    cfg->layer3.cfg_tile.Tm = 1;
    cfg->layer3.cfg_tile.Tr = 1;
    cfg->layer3.cfg_tile.Tc = 1;

    cfg->layer3.cfg_k.K = 5;
    cfg->layer3.cfg_k.M = 16;
    cfg->layer3.cfg_k.stride = 1;

    init_conv2d_config(&cfg->layer3, cfg->layer3.cfg_input, cfg->layer3.cfg_k, cfg->layer3.cfg_tile);



    cfg->layer4.cfg_input.X = 8;
    cfg->layer4.cfg_input.Y = 8;
    cfg->layer4.cfg_input.Z = 16;

    cfg->layer4.cfg_k.K = 2;
    cfg->layer4.cfg_k.M = 16;
    cfg->layer4.cfg_k.stride = 2;

    cfg->layer4.cfg_output.C = 4;
    cfg->layer4.cfg_output.R = 4;
    cfg->layer4.cfg_output.M = 16;

    init_pool_config(&cfg->layer4, cfg->layer4.cfg_input, cfg->layer4.cfg_output, cfg->layer4.cfg_k);

    cfg->layer5.input_size = 4 * 4 * 16;
    cfg->layer5.output_size = 120;
    init_fully_connected_config(&cfg->layer5, cfg->layer5.input_size, cfg->layer5.output_size);

    cfg->layer6.input_size = 120;
    cfg->layer6.output_size = 84;
    init_fully_connected_config(&cfg->layer6, cfg->layer6.input_size, cfg->layer6.output_size);

    cfg->output_layer.input_size = 84;
    cfg->output_layer.output_size = 10;
    init_fully_connected_config(&cfg->output_layer, cfg->output_layer.input_size, cfg->output_layer.output_size);
}



int max_likelihood(fc_t *y)
{
    int i_likely = 0;
    fc_t y_max = 0;
    for (int i = 0; i < 10; i++)
    {
        if (y[i] > y_max)
        {
            y_max = y[i];
            i_likely = i;
        }
    }
    return i_likely;
}

int lenet5_inference(lenet5_config* cfg, lenet5_t* image)
{
	conv_t *output1 = conv_layer(&cfg->layer1, image, w1, b1);
	pool_t *output2 = pool_layer(&cfg->layer2, output1);
	conv_t* output3 = conv_layer(&cfg->layer3, output2, w3, b3);
	pool_t* output4 = pool_layer(&cfg->layer4, output3);
	fc_t* output5 = fully_connected_layer(&cfg->layer5, output4, w5, b5);
	fc_t* output6 = fully_connected_layer(&cfg->layer6, output5, w6, b6);
	fc_t* output7 = fully_connected_layer(&cfg->output_layer, output6, w7, b7);
    int prediction = max_likelihood(output7);


	free(output1);
	free(output2);
	free(output3);
	free(output4);
	free(output5);
	free(output6);
	free(output7);
	return prediction;
}


int lenet5_inference_mac(lenet5_config* cfg, sc_mac_settings *sc_cfg, lenet5_t* image)
{
		conv_t *output1 = conv_layer_mac_acc(&cfg->layer1, sc_cfg, image, w1, b1);
		pool_t *output2 = pool_layer(&cfg->layer2, output1);
		conv_t* output3 = conv_layer(&cfg->layer3, output2, w3, b3);
		pool_t* output4 = pool_layer(&cfg->layer4, output3);
		fc_t* output5 = fully_connected_layer(&cfg->layer5, output4, w5, b5);
		fc_t* output6 = fully_connected_layer(&cfg->layer6, output5, w6, b6);
		fc_t* output7 = fully_connected_layer(&cfg->output_layer, output6, w7, b7);
	    int prediction = max_likelihood(output7);

	    free(output1);
	    free(output2);
	    free(output3);
	    free(output4);
	    free(output5);
		free(output6);
		free(output7);
		return prediction;
}
