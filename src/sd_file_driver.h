/*
 * sd_file_driver.h
 *
 *  Created on: 15 Nov 2020
 *      Author: Jacob
 */

#ifndef SRC_SD_FILE_DRIVER_H_
#define SRC_SD_FILE_DRIVER_H_
#include "platform.h"
#include "xparameters.h"
#include "xdevcfg.h"

int SD_Init();
int SD_Transfer_read(char *FileName,u32 DestinationAddress,u32 ByteLength);
int SD_Transfer_write(char *FileName,u32 SourceAddress,u32 ByteLength);
int get_mnist_image(int index, u8 *image_data, u32 *label );
int get_mnist_image_float(int index, float *image_data, u32 *label );
void print_mnist_image(u8 *image_data, u32 label);

#endif /* SRC_SD_FILE_DRIVER_H_ */
