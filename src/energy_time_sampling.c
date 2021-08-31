/*
 * energy_time_sampling.c
 *
 *  Created on: 30 Nov 2020
 *      Author: Jacob
 */

#include <stdio.h>
#include "energy_time_sampling.h"
#include "xparameters.h"   /* SDK generated parameters */
#include "xuartps.h"
#include "xil_printf.h"
#include "ff.h"
#include "xil_cache.h"
#include "xplatform_info.h"

#include "xtime_l.h"
#include "xadcps_hw.h"
#include "xadcps.h"


#define XADC_DEVICE_ID				XPAR_PS7_XADC_0_DEVICE_ID
XTime tStart, tEnd;
    long double tElapsed;

long double count = 0;
XAdcPs_Config *Adc_Config;
u32 VCCINTraw, VBRAMraw;
float VCCINT_val, VBRAM_val;
float P_inst = 0, P_max = 0, P_total=0, E_total = 0;
unsigned int MinutesPassed = 1;
static XAdcPs Adc_Struct;
int Status;

int init_adc()
{
	Adc_Config = XAdcPs_LookupConfig(XADC_DEVICE_ID);
	if (Adc_Config == NULL) {
		return XST_FAILURE;
	}

	Status = XAdcPs_CfgInitialize(&Adc_Struct, Adc_Config, Adc_Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("ERROR: XADC Initialization Failed\r\n");
		return XST_FAILURE;
	}

	Status = XAdcPs_SelfTest(&Adc_Struct);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	// Set the sampling mode
	XAdcPs_SetSequencerMode(&Adc_Struct, XADCPS_SEQ_MODE_CONTINPASS);
	// Set the alarm enable, directly off
	XAdcPs_SetAlarmEnables(&Adc_Struct, 0x0);
	// Set the enable sampling channel
	XAdcPs_SetSeqChEnables(&Adc_Struct, XADCPS_CH_VCCINT|XADCPS_CH_VBRAM);
	return XST_SUCCESS;
}

int init_time()
{
	XTime_GetTime(&tStart);
	return 0;
}
void sample_adc()
{
	VCCINTraw  = XAdcPs_GetAdcData(&Adc_Struct, XADCPS_CH_VCCINT);
	VCCINT_val = XAdcPs_RawToVoltage(VCCINTraw);
	VBRAMraw   = XAdcPs_GetAdcData(&Adc_Struct, XADCPS_CH_VBRAM);
	VBRAM_val  = XAdcPs_RawToVoltage(VBRAMraw);
	P_inst     = VCCINT_val*(VCCINT_val - 1.01 + 0.05) + VBRAM_val*(VBRAM_val - 1.01 + 0.005) ;
	if (P_max < P_inst) {
		P_max = P_inst;
	}

	P_total += P_inst;
	count    = count+1;

	if (tElapsed >= 60*MinutesPassed) {
	    E_total += P_total / count;
	    if (MinutesPassed > 1)
	        E_total = E_total/2;
	    MinutesPassed = MinutesPassed + 1;
	    P_total = 0;
	    count = 0;
	 }

	if (E_total == 0) {
		E_total = P_total/count * tElapsed;
	}
	else {
		E_total = (E_total + P_total/count)/2*tElapsed;
	}
}

void sample_time()
{
	XTime_GetTime(&tEnd);
    tElapsed = (long double)((tEnd-tStart) *2)/(long double)XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ;
}

void print_stats(int cycles)
{

	printf("Total energy :   %f J\n", E_total);
	printf("Peak power   :   %f mW\n", P_max*1000);
	printf("P_total      :   %f  \n", P_total);
	printf("Cycles       :   %d   \n", cycles);
	float time = (float)cycles/(float)1000000;
	printf("tElapsed     :   %f    \n", tElapsed);
	printf("Conv Time    :   %f   \n", (float)(cycles*16)/(float)1000000);
	printf("Conv Tile T  :   %f   \n", (float)cycles/(float)1000000);
	printf("Conv Energy  :   %f   \n", P_total/count *(time*16));

}
