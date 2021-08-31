#include <string.h>
#include "platform.h"
#include "xparameters.h"
#include <stdio.h>
#include "xil_printf.h"
#include "ff.h"
#include "xdevcfg.h"
#include "sd_file_driver.h"
#include "LeNet5.h"
#include "sc_mac_driver.h"
#include "energy_time_sampling.h"

u8 image_data [28*28];
u32 label;
int number_of_images = 10000;


float fl_image_data[28*28];

lenet5_config cfg;
sc_mac_settings sc_cfg;


void progress_text(int current_image, int total)
{
	if(current_image%25 == 0)
	{
		printf("Progess %f\n", ((float)current_image/(float)total)*100);
	}
}

void print_results(int correct, int sample_size)
{
	  printf("total %d correct %d percentage %f\n", sample_size, correct, (float)correct/(float) sample_size);
}

void lenet5_inference_software(int sample_size)
{
	int label;
	int correct = 0;
	for(int i = 0; i< 0; i++)
	{
		progress_text(i, sample_size);
	    get_mnist_image_float(i, fl_image_data, &label);
	    int pred = lenet5_inference(&cfg, fl_image_data);
	    if(pred == label)
	    {
	    		correct++;
	    }
	}
	print_results(correct, sample_size);
}

void lenet5_inference_hw_acceleration(int sample_size)
{
	int label;
	int correct = 0;
	for(int i = 0; i < sample_size; i++)
	{
		progress_text(i, sample_size);
		get_mnist_image_float(i, fl_image_data, &label);
	    init_time();
	    int pred = lenet5_inference_mac(&cfg, &sc_cfg,fl_image_data);
	    sample_time();
	    sample_adc();
	    if(pred == label)
	    {
	       	correct++;
	    }
	}
	print_results(correct, sample_size);
}
int main()
{


    lenet5_config_init(&cfg);
    print("SD write and read start!\r\n");


    test_sc_mac_get_config();
    init_adc();
    lenet5_config_init(&cfg);
    init_sc_mac_settings(&sc_cfg, XPAR_SC_CONV_LAYER_MAC_0_S00_AXI_BASEADDR);
    set_tile_from_mac(&sc_cfg, &cfg.layer1);

    lenet5_inference_software(number_of_images);
    lenet5_inference_hw_acceleration(number_of_images);

    print("SD write and read over!\r\n");
    print_stats(sc_cfg.cycles);

    return 0;
}
