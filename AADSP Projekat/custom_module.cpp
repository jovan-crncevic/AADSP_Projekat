#include "custom_module.h"
#include <math.h>

void CustomModule_InitializeArgumentsTable(CustomModule_ArgumentsTable* argumentsTable, int module_enabled, double input_gain, int output_mode)
{
	argumentsTable->module_enabled = module_enabled;
	argumentsTable->input_gain = input_gain;
	argumentsTable->output_mode = output_mode;
}

double FIR_history[FIR_N_COEFF] = { 0 };

double FIR_coefficients[FIR_N_COEFF] = {
    -0.00176228506971007060,
    -0.00178903340141017310,
    -0.00181560139186307300,
    -0.00184198031306316720,
    -0.00186816140555434000,
    -0.00189413588176219230,
    -0.00191989492938809960,
    -0.00194542971486044500,
    -0.00197073138684363380,
    -0.00199579107979875380,
    -0.00202059991760353710,
    -0.00204514901722555560,
    -0.00206942949244715320,
    -0.00209343245764618580,
    -0.00211714903162275700,
    -0.00214057034148408020,
    -0.00216368752657050560,
    -0.00218649174244151590,
    -0.00220897416489539100,
    -0.00223112599404714320,
    -0.00225293845844519060,
    -0.00227440281923629300,
    -0.00229551037436967390,
    -0.00231625246284685020,
    -0.00233662046900991400,
    -0.00235660582686900460,
    -0.00237620002446867290,
    -0.00239539460829554880,
    -0.00241418118770594980,
    -0.00243255143940985210,
    -0.00245049711196973930,
    -0.00246801003033778620,
    -0.00248508210042246160,
    -0.00250170531368076570,
    -0.00251787175174006350,
    -0.00253357359104257200,
    -0.00254880310751466050,
    -0.00256355268125522260,
    -0.00257781480124945250,
    -0.00259158207009424970,
    -0.00260484720874450990,
    -0.00261760306127441610,
    -0.00262984259964800120,
    -0.00264155892850591500,
    -0.00265274528995751350,
    -0.00266339506838207460,
    -0.00267350179523591790,
    -0.00268305915386266680,
    -0.00269206098430466730,
    -0.00270050128811551460,
    -0.00270837423316832880,
    -0.00271567415846187810,
    -0.00272239557891816280,
    -0.00272853319017408000,
    -0.00273408187335893270,
    -0.00273903669986264200,
    -0.00274339293608723630,
    -0.00274714604818192610,
    -0.00275029170675838510,
    -0.00275282579158504450,
    -0.00275474439625718720,
    -0.00275604383284208620,
    -0.00275672063649486530,
    0.99722959522149091000,
    -0.00275672063649486530,
    -0.00275604383284208620,
    -0.00275474439625718720,
    -0.00275282579158504450,
    -0.00275029170675838510,
    -0.00274714604818192610,
    -0.00274339293608723630,
    -0.00273903669986264200,
    -0.00273408187335893270,
    -0.00272853319017408000,
    -0.00272239557891816280,
    -0.00271567415846187810,
    -0.00270837423316832880,
    -0.00270050128811551460,
    -0.00269206098430466730,
    -0.00268305915386266680,
    -0.00267350179523591790,
    -0.00266339506838207460,
    -0.00265274528995751350,
    -0.00264155892850591500,
    -0.00262984259964800120,
    -0.00261760306127441610,
    -0.00260484720874450990,
    -0.00259158207009424970,
    -0.00257781480124945250,
    -0.00256355268125522260,
    -0.00254880310751466050,
    -0.00253357359104257200,
    -0.00251787175174006350,
    -0.00250170531368076570,
    -0.00248508210042246160,
    -0.00246801003033778620,
    -0.00245049711196973930,
    -0.00243255143940985210,
    -0.00241418118770594980,
    -0.00239539460829554880,
    -0.00237620002446867290,
    -0.00235660582686900460,
    -0.00233662046900991400,
    -0.00231625246284685020,
    -0.00229551037436967390,
    -0.00227440281923629300,
    -0.00225293845844519060,
    -0.00223112599404714320,
    -0.00220897416489539100,
    -0.00218649174244151590,
    -0.00216368752657050560,
    -0.00214057034148408020,
    -0.00211714903162275700,
    -0.00209343245764618580,
    -0.00206942949244715320,
    -0.00204514901722555560,
    -0.00202059991760353710,
    -0.00199579107979875380,
    -0.00197073138684363380,
    -0.00194542971486044500,
    -0.00191989492938809960,
    -0.00189413588176219230,
    -0.00186816140555434000,
    -0.00184198031306316720,
    -0.00181560139186307300,
    -0.00178903340141017310,
    -0.00176228506971007060,
    -0.00173536509003662640
};

static void CustomModule_FIR(const double tempBuffer[BLOCK_SIZE], double sampleBuffer[MAX_NUM_CHANNEL][BLOCK_SIZE])
{
	for (int k = 0; k < BLOCK_SIZE; k++)
	{
		for (int i = FIR_N_COEFF - 2; i >= 0; i--)
		{
			FIR_history[i + 1] = FIR_history[i];
		}

		FIR_history[0] = tempBuffer[k];

		sampleBuffer[2][k] = 0;

		for (int j = 0; j < FIR_N_COEFF; j++)
		{
			sampleBuffer[2][k] += FIR_coefficients[j] * FIR_history[j];
		}
	}
}

static void CustomModule_Distorsion(const double tempBuffer[BLOCK_SIZE], double sampleBuffer[MAX_NUM_CHANNEL][BLOCK_SIZE])
{
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        if (tempBuffer[i] > DISTORSION_TRESHOLD)
        {
            sampleBuffer[5][i] = DISTORSION_TRESHOLD;
        }
        else if (tempBuffer[i] < -1 * DISTORSION_TRESHOLD)
        {
            sampleBuffer[5][i] = -1 * DISTORSION_TRESHOLD;
        }
        else
        {
            sampleBuffer[5][i] = tempBuffer[i];
        }
    }
}

static void CustomModule_L_R(const double input_gain, double sampleBuffer[MAX_NUM_CHANNEL][BLOCK_SIZE])
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		sampleBuffer[0][i] *= pow(10, input_gain / 20.0);
		sampleBuffer[1][i] *= pow(10, input_gain / 20.0);
	}
}

static void CustomModule_Ls_Rs(double sampleBuffer[MAX_NUM_CHANNEL][BLOCK_SIZE])
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		sampleBuffer[3][i] = -1 * pow(10, 3 / 20.0) * sampleBuffer[0][i];
		sampleBuffer[4][i] = -1 * pow(10, 3 / 20.0) * sampleBuffer[1][i];
	}
}

static void CustomModule_C_LFE(double sampleBuffer[MAX_NUM_CHANNEL][BLOCK_SIZE])
{
	double tempBuffer[BLOCK_SIZE] = { 0 };

	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		tempBuffer[i] = sampleBuffer[0][i] + sampleBuffer[1][i];
	}

	CustomModule_FIR(tempBuffer, sampleBuffer);
	CustomModule_Distorsion(tempBuffer, sampleBuffer);
}

void CustomModule_Main(CustomModule_ArgumentsTable argumentsTable, double sampleBuffer[MAX_NUM_CHANNEL][BLOCK_SIZE])
{
	if (argumentsTable.module_enabled)
	{
		CustomModule_L_R(argumentsTable.input_gain, sampleBuffer);
		CustomModule_Ls_Rs(sampleBuffer);
		CustomModule_C_LFE(sampleBuffer);
	}
}