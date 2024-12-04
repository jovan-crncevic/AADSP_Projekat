#include "custom_module.h"
#include <iostream>

void CustomModule_InitializeArgumentsTable(CustomModule_ArgumentsTable* argumentsTable, int module_enabled, double input_gain, int output_mode)
{
	argumentsTable->module_enabled = module_enabled;
	argumentsTable->input_gain = input_gain;
	argumentsTable->output_mode = output_mode;
}

void CustomModule_InputGain()
{

}

void CustomModule_Main(CustomModule_ArgumentsTable argumentsTable, double sampleBuffer[][], double outputBuffer[][])
{
	if (argumentsTable.module_enabled)
	{
		CustomModule_InputGain()
	}
}