/*
 * LeNet5.h
 *
 *  Created on: 19 Nov 2020
 *      Author: Jacob
 */

#ifndef SRC_LENET5_H_
#define SRC_LENET5_H_

#include "nn_utils.h"
#include "sc_mac_driver.h"

typedef struct
{
    conv2d_config layer1;
    pool_config layer2;
    conv2d_config layer3;
    pool_config layer4;
    fully_connected_config layer5;
    fully_connected_config layer6;
    fully_connected_config output_layer;
} lenet5_config;

typedef float lenet5_t;


int lenet5_inference(lenet5_config* cfg, lenet5_t* image);
void lenet5_config_init(lenet5_config *cfg);
int lenet5_inference_mac(lenet5_config* cfg, sc_mac_settings *sc_cfg, lenet5_t* image);


#endif /* SRC_LENET5_H_ */
