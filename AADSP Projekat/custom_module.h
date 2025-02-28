#ifndef __CUSTOM_MODULE_H__
#define __CUSTOM_MODULE_H__

#define BLOCK_SIZE 16
#define MAX_NUM_CHANNEL 8
#define FIR_N_COEFF 128
#define DISTORSION_TRESHOLD 0.1

//////////////////////////////
//	module_enabled:			//
//		0 - disabled		//
//		1 - enabled			//
//		default: 1			//
//							//
//	input_gain [dB]:		//
//		0 - max				//
//		-INF - min			//
//		default: -6			//
//							//
//	output_mode:			//
//		0 - 2_2_0			//
//		1 - 2_2_1			//
//		2 - 3_2_0			//
//		3 - 3_2_1			//
//		default: 3			//
//////////////////////////////
typedef struct {
	int module_enabled = 1;
	double input_gain = -6;
	int output_mode = 3;
} CustomModule_ArgumentsTable;

void CustomModule_InitializeArgumentsTable(CustomModule_ArgumentsTable*, int, double, int);
void CustomModule_Main(CustomModule_ArgumentsTable, double sampleBuffer[MAX_NUM_CHANNEL][BLOCK_SIZE]);

#endif