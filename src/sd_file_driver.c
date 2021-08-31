#include "sd_file_driver.h"
#include <string.h>
#include <stdio.h>
#include "platform.h"
#include "xparameters.h"

#include "xil_printf.h"
#include "ff.h"
#include "xdevcfg.h"


static FATFS fatfs;
char FILE_NAME[32];

int SD_Init()
{
    FRESULT rc;

    rc = f_mount(&fatfs,"",0);
    if(rc)
    {
        xil_printf("ERROR : f_mount returned %d\r\n",rc);
        return XST_FAILURE;
    }
    return XST_SUCCESS;
}

int SD_Transfer_read(char *FileName,u32 DestinationAddress,u32 ByteLength)
{
    FIL fil;
    FRESULT rc;
    UINT br;

    rc = f_open(&fil,FileName,FA_READ);
    if(rc)
    {
        xil_printf("ERROR : f_open returned %d\r\n",rc);
        return XST_FAILURE;
    }
    rc = f_lseek(&fil, 0);
    if(rc)
    {
        xil_printf("ERROR : f_lseek returned %d\r\n",rc);
        return XST_FAILURE;
    }
    rc = f_read(&fil, (void*)DestinationAddress,ByteLength,&br);
    if(rc)
    {
        xil_printf("ERROR : f_read returned %d\r\n",rc);
        return XST_FAILURE;
    }
    rc = f_close(&fil);
    if(rc)
    {
        xil_printf(" ERROR : f_close returned %d\r\n", rc);
        return XST_FAILURE;
    }
    return XST_SUCCESS;
}

int SD_Transfer_write(char *FileName,u32 SourceAddress,u32 ByteLength)
{
    FIL fil;
    FRESULT rc;
    UINT bw;

    rc = f_open(&fil,FileName,FA_CREATE_ALWAYS | FA_WRITE);
    if(rc)
    {
        xil_printf("ERROR : f_open returned %d\r\n",rc);
        return XST_FAILURE;
    }
    rc = f_lseek(&fil, 0);
    if(rc)
    {
        xil_printf("ERROR : f_lseek returned %d\r\n",rc);
        return XST_FAILURE;
    }
    rc = f_write(&fil,(void*) SourceAddress,ByteLength,&bw);
    if(rc)
    {
        xil_printf("ERROR : f_write returned %d\r\n", rc);
        return XST_FAILURE;
    }
    rc = f_close(&fil);
    if(rc){
        xil_printf("ERROR : f_close returned %d\r\n",rc);
        return XST_FAILURE;
    }
    return XST_SUCCESS;
}


u8 temp_buffer[28*28+1]; //IMAGE SIZE 28*28, 1 FOR LABEL
int get_mnist_image(int index, u8 *image_data, u32 *label )
{
	int STATUS;
	sprintf(FILE_NAME,"i%d.bin",index);
	SD_Init();
	STATUS = SD_Transfer_read(FILE_NAME,(u32)temp_buffer,(28*28+1));
	*label = temp_buffer[0];
	for(int i = 0; i < 28*28; i++)
		image_data[i] = temp_buffer[i+1];
	return STATUS;
}

void print_float_array(float *array, u32 label)
{
	printf("label %d\n", label);
	for(int i = 0; i < 28*28; i++)
	{
		printf("%f ", array[i]);
		if(i%28 == 27)
		{
				printf("\n");
		}
	}
}


int get_mnist_image_float(int index, float *image_data, u32 *label )
{
	int STATUS;
	sprintf(FILE_NAME,"i%d.bin",index);
	SD_Init();
	STATUS = SD_Transfer_read(FILE_NAME,(u32)temp_buffer,(28*28+1));
	*label = temp_buffer[0];
	for(int i = 0; i < 28*28; i++)
		image_data[i] = (float)temp_buffer[i+1]/(float)256;
	//print_float_array(image_data, *label);
	return STATUS;
}



void print_mnist_image(u8 *image_data, u32 label)
{
	printf("label %d\n", label);
	for(int i = 0; i < 28*28; i++)
	{
		printf("%u ", image_data[i]);
		if(i%28 == 27)
		{
			printf("\n");
		}
	}
}
